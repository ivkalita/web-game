#include "WebgameServer.hpp"
#include "Router.hpp"
#include "DBConnector.hpp"

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include "Poco/Net/HTMLForm.h"

#include "Poco/Random.h"
#include "Poco/PBKDF2Engine.h"
#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"

#include <iomanip>

static Poco::Random rnd;

static std::string getRandomString(int len = 64) {
    std::stringstream res;
    for (int i = 0; i < (int)ceil(len / 8); i++)
        res << std::setfill('0') << std::setw(8) << std::hex << rnd.next();
    return res.str().substr(0, len);
}

#define SALT_LENGTH 64

static std::string getPasswordHash(std::string password, std::string salt = getRandomString(SALT_LENGTH)) {
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> hs(salt, 4096);
    hs.update(password);
    return salt + ":" + hs.digestToHex(hs.digest());
}

static Poco::JSON::Query getJSONQuery(const RouteMatch& m) {
    Poco::Net::HTMLForm form(m.request(), m.request().stream());
    Poco::JSON::Parser p;
    Poco::Dynamic::Var var(p.parse(form.get("jsonObj")));
    return Poco::JSON::Query(var);
}

static void returnResponse(const RouteMatch& m, const std::string result, const std::string data = "null") {
    std::ostream& st = m.response().send();
    st << "{\"result\":\"" + result + "\",\"data\":"+data+"}";
    st.flush();
}

static void returnResponse(const RouteMatch& m, const std::string result, const Poco::Dynamic::Var data) {
    std::ostream& st = m.response().send();
    st << "{\"result\":\"" + result + "\",\"data\":";
    Poco::JSON::Stringifier::condense(data, st);
    st << "}";
    st.flush();
}

static void Login(const RouteMatch& m) {
    std::string login, name, password;
    try {
        Poco::JSON::Query query(getJSONQuery(m));
        query.find("login").convert(login);
        query.find("password").convert(password);
    }
    catch (...) {
        return returnResponse(m, "BadRequest");
    }
    try {
        auto user = DBConnection::instance().ExecParams("SELECT password, name FROM users WHERE login=$1", { login });
        bool valid = user.row_count();
        if (valid) {
            std::string passhash((*user.begin()).field_by_name("password"));
            valid &= !getPasswordHash(password, passhash.substr(0, SALT_LENGTH)).compare(passhash);
        }
        if (!valid) return returnResponse(m, "BadCredentials");
        std::string token = getRandomString();
        DBConnection::instance().ExecParams("UPDATE users SET token = $1 WHERE login = $2", { token, login });
        Poco::JSON::Object res;
        res.set("name", (*user.begin()).field_by_name("name"));
        res.set("accessToken", token);
        return returnResponse(m, "Ok", res);
    }
    catch (...) {
        return returnResponse(m, "InternalError");
    }
}

static void Logout(const RouteMatch& m) {
    std::string accessToken;
    try {
        getJSONQuery(m).find("accessToken").convert(accessToken);
    }
    catch (...) {
        return returnResponse(m, "BadRequest");
    }
    try {
        auto res = DBConnection::instance().ExecParams("SELECT token FROM users WHERE token=$1", { accessToken });
        if (res.row_count()) {
            DBConnection::instance().ExecParams("UPDATE users SET token = '' WHERE token = $1", { accessToken });
            return returnResponse(m, "Ok");
        }
        return returnResponse(m, "NotLoggedIn");
    }
    catch (...) {
        return returnResponse(m, "InternalError");
    }
}

static void Register(const RouteMatch& m) {
    std::string login, name, password;
    try {
        Poco::JSON::Query query(getJSONQuery(m));
        query.find("login").convert(login);
        query.find("name").convert(name);
        query.find("password").convert(password);
    }
    catch (...) {
        return returnResponse(m, "BadRequest");
    }
    try {
        if (login.length() < 2 || login.length() > 36)
            return returnResponse(m, "BadLogin");
        if (name.length() < 2 || name.length() > 36)
            return returnResponse(m, "BadName");
        if (password.length() < 2 || password.length() > 36)
            return returnResponse(m, "BadPassword");
        auto res = DBConnection::instance().ExecParams("SELECT login FROM users WHERE login=$1", { login });
        if (res.row_count())
            return returnResponse(m, "LoginExists");
        std::string token = getRandomString();
        DBConnection::instance().ExecParams("INSERT INTO users (login, name, password, token) values ($1, $2, $3, $4)",
            { login, name, getPasswordHash(password), token });
        return returnResponse(m, "Ok", "{\"accessToken\":\""+token+"\"}");
    }
    catch (...) {
        return returnResponse(m, "InternalError");
    }
}

class Users {
public:
    Users() {
        rnd.seed();
        auto & router = Router::instance();
        router.registerRoute("POST", "/api/login", Login);
        router.registerRoute("POST", "/api/register", Register);
        router.registerRoute("POST", "/api/logout", Logout);
    }
};

static Users users;

