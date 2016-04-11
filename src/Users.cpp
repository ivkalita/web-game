#include "WebsocketRouter.hpp"
#include "DBConnector.hpp"
#include "User.hpp"

#include "Poco/Random.h"
#include "Poco/PBKDF2Engine.h"
#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"

User::User(std::string field, std::string value) {
    auto user = DBConnection::instance().ExecParams("SELECT id, name FROM users WHERE " + field + "=$1", { value });
    if (user.row_count() != 1) throw std::exception();
    id = stoi((*user.begin()).field_by_name("id"));
    name = (*user.begin()).field_by_name("name");
}

#include <iomanip>

namespace {
    Poco::Random rnd;

    std::string genRandomString(int len = 64) {
        std::stringstream res;
        for (int i = 0; i < (int)ceil(len / 8); i++)
            res << std::setfill('0') << std::setw(8) << std::hex << rnd.next();
        return res.str().substr(0, len);
    }

#define SALT_LENGTH 64

    std::string getPasswordHash(std::string password, std::string salt = genRandomString(SALT_LENGTH)) {
        Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> hs(salt, 4096);
        hs.update(password);
        return salt + ":" + hs.digestToHex(hs.digest());
    }

    Poco::JSON::Object Login(Poco::JSON::Object & params, int & userId) {
        std::string login, password;
        login = params.get("login").convert<std::string>();
        password = params.get("password").convert<std::string>();

        Poco::JSON::Object r;

        auto user = DBConnection::instance().ExecParams("SELECT id, password, name FROM users WHERE login=$1", { login });
        if (user.row_count() != 1) {
            r.set("result", "badCredentials");
            return r;
            //return jsonDataResult("badCredentials");
        }
        std::string passhash = (*user.begin()).field_by_name("password");
        if (getPasswordHash(password, passhash.substr(0, SALT_LENGTH)).compare(passhash)) {
            r.set("result", "badCredentials");
            return r;
            //return jsonDataResult("badCredentials");
        }

        std::string token = genRandomString();
        userId = std::stoi((*user.begin()).field_by_name("id"));
        DBConnection::instance().ExecParams("UPDATE users SET online = true, token = $1 WHERE id = $2", { token, std::to_string(userId) });

        r.set("token", token);
        r.set("name", (*user.begin()).field_by_name("name"));
        return r;
    }

#define VALUE_MAX_LENGTH 256

    Poco::JSON::Object Register(Poco::JSON::Object & params, int & user) {
        std::string login, password, name;
        login = params.get("login").convert<std::string>();
        password = params.get("password").convert<std::string>();
        name = params.get("name").convert<std::string>();

        Poco::JSON::Object r;
        Poco::JSON::Array errors;
        if (login.length() == 0) errors.add("loginRequired");
        if (login.length() > VALUE_MAX_LENGTH) errors.add("loginTooLong");
        if (DBConnection::instance().ExecParams("SELECT id FROM users WHERE login=$1", { login }).row_count() != 0) errors.add("loginExists");
        if (password.length() == 0) errors.add("passwordRequired");
        if (password.length() > VALUE_MAX_LENGTH) errors.add("passwordTooLong");
        if (name.length() == 0) errors.add("nameRequired");
        if (name.length() > VALUE_MAX_LENGTH) errors.add("nameTooLong");
        if (errors.size()) {
            r.set("errors", errors);
            return r;
        }

        std::string token = genRandomString();
        DBConnection::instance().ExecParams("INSERT INTO users (login, name, password, token, online) values ($1, $2, $3, $4, true)", {
            login, name, getPasswordHash(password), token
        });
        auto userR = DBConnection::instance().ExecParams("SELECT id FROM users WHERE login=$1", { login });
        user = std::stoi((*userR.begin()).field_by_name("id"));

        r.set("token", token);
        return r;
    }

    Poco::JSON::Object Logout(Poco::JSON::Object & params, int & user) {
        DBConnection::instance().ExecParams("UPDATE users SET online = false WHERE id = $1", { std::to_string(user) });
        user = -1;
        return Poco::JSON::Object();
    }

    Poco::JSON::Object Token(Poco::JSON::Object & params, int & userId) {
        std::string token;
        token = params.get("token").convert<std::string>();

        Poco::JSON::Object r;

        auto user = DBConnection::instance().ExecParams("SELECT id, name FROM users WHERE token=$1", { token });
        if (user.row_count() != 1) {
            r.set("result", "badToken");
            return r;
        }

        userId = std::stoi((*user.begin()).field_by_name("id"));
        DBConnection::instance().ExecParams("UPDATE users SET online = true WHERE id = $1", { std::to_string(userId) });

        r.set("name", (*user.begin()).field_by_name("name"));
        return r;
    }

    Poco::JSON::Object Settings(Poco::JSON::Object & params, int & user) {
        std::string name;
        name = params.get("name").convert<std::string>();

        Poco::JSON::Object r;
        Poco::JSON::Array errors;
        if (name.length() == 0) errors.add("nameRequired");
        if (name.length() > VALUE_MAX_LENGTH) errors.add("nameTooLong");
        if (errors.size()) {
            r.set("errors", errors);
            return r;
        }

        DBConnection::instance().ExecParams("UPDATE users SET name = $1 WHERE id = $2", { name, std::to_string(user) });

        return r;
    }

    Poco::JSON::Object Password(Poco::JSON::Object & params, int & user) {
        std::string passwordOld, password;
        passwordOld = params.get("passwordOld").convert<std::string>();
        password = params.get("password").convert<std::string>();

        Poco::JSON::Object r;

        auto userR = DBConnection::instance().ExecParams("SELECT password FROM users WHERE id=$1", { std::to_string(user) });
        std::string passhash = (*userR.begin()).field_by_name("password");
        if (getPasswordHash(passwordOld, passhash.substr(0, SALT_LENGTH)).compare(passhash)) {
            r.set("result", "invalidPassword");
            return r;
        }

        DBConnection::instance().ExecParams("UPDATE users SET password = $1 WHERE id = $2", { getPasswordHash(password), std::to_string(user) });

        return r;
    }

    class Pages {
    public:
        Pages() {
            rnd.seed();
            auto & router = WebsocketRouter::instance();
            router.registerRoute("user/login", Login, false);
            router.registerRoute("user/token", Token, false);
            router.registerRoute("user/register", Register, false);
            router.registerRoute("user/logout", Logout);
            router.registerRoute("user/settings", Settings);
            router.registerRoute("user/password", Password);
        }
    };

    Pages pages;
}
