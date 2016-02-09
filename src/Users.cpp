#include "WebgameServer.hpp"
#include "Router.hpp"
#include "DBConnector.hpp"
#include "User.hpp"
#include "MyUtils.hpp"
#include "JSONRequestHandler.hpp"

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

static const int SALT_LENGTH = 64;

static std::string getPasswordHash(std::string password, std::string salt = MyUtils::genRandomString(SALT_LENGTH)) {
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> hs(salt, 4096);
    hs.update(password);
    return salt + ":" + hs.digestToHex(hs.digest());
}

static void Login(JsonRequest & rq, JsonResponse & rs) {
    std::string login, password;
    rq.get("login", login);
    rq.get("password", password);
    
    auto user = DBConnection::instance().ExecParams("SELECT password, name FROM users WHERE login=$1", { login });
    if (user.row_count() != 1) return rs.send("BadCredentials");
    std::string passhash = (*user.begin()).field_by_name("password");
    if (getPasswordHash(password, passhash.substr(0, SALT_LENGTH)).compare(passhash))
        return rs.send("BadCredentials");
    std::string token = MyUtils::genRandomString();
    DBConnection::instance().ExecParams("UPDATE users SET token = $1 WHERE login = $2", { token, login });
    std::string name = (*user.begin()).field_by_name("name");
    rs.set("name", name);
    rs.set("accessToken", token);
    rs.send("Ok");
}

static void Logout(JsonRequest & rq, JsonResponse & rs) {
    std::string token;
    rq.get("accessToken", token);

    auto res = DBConnection::instance().ExecParams("SELECT token FROM users WHERE token=$1", { token });
    if (!res.row_count()) return rs.send("NotLoggedIn");
    DBConnection::instance().ExecParams("UPDATE users SET token = '' WHERE token = $1", { token });
    rs.send("Ok");
}

static void Register(JsonRequest & rq, JsonResponse & rs) {
    std::string login, password, name;
    rq.get("login", login);
    rq.get("password", password);
    rq.get("name", name);

    if (login.length() < 2 || login.length() > 36) return rs.send("BadLogin");
    if (DBConnection::instance().ExecParams("SELECT login FROM users WHERE login=$1", { login }).row_count() != 0) return rs.send("LoginExists");
    if (password.length() < 2 || password.length() > 36) return rs.send("BadPassword");
    if (name.length() < 2 || name.length() > 36) return rs.send("BadName");

    std::string token = MyUtils::genRandomString();
    DBConnection::instance().ExecParams("INSERT INTO users (login, name, password, token) values ($1, $2, $3, $4)", {
        login, name, getPasswordHash(password), token
    });

    rs.set("accessToken", token);
    rs.send("Ok");
}


class Users {
public:
    Users() {
        auto & router = Router::instance();
        router.registerRoute("POST", "/api/login", CatchException<Login>);
        router.registerRoute("POST", "/api/register", CatchException<Register>);
        router.registerRoute("POST", "/api/logout", CatchException<Logout>);
    }
};

static Users users;

