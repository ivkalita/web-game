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

static std::string genRandomString(int len = 64) {
    std::stringstream res;
    for (int i = 0; i < (int)ceil(len / 8); i++)
        res << std::setfill('0') << std::setw(8) << std::hex << rnd.next();
    return res.str().substr(0, len);
}

class JsonRequest {
    Poco::Dynamic::Var v;
    const RouteMatch& _m;
public:
    JsonRequest(const RouteMatch& m) : _m(m) {
        Poco::Net::HTMLForm form(m.request(), m.request().stream());
        Poco::JSON::Parser p;
        v = p.parse(form.get("jsonObj"));
    }

    template <typename T>
    void get(const std::string & name, T & value) const {
        Poco::JSON::Query q(v);
        try {
            q.find(name).convert(value);
        }
        catch (...) {
            JsonResponse(_m, "BadRequest").send();
        }
    }
};

class JsonResponse {
    const RouteMatch & _m;
    std::string _result;
    Poco::JSON::Object data;
public:
    JsonResponse(const RouteMatch& m, const std::string result): _m(m), _result(result) {}

    template <typename T>
    void set(const std::string & name, T & value) {
        data.set(name, value);
    }

    void send() {
        std::ostream& st = _m.response().send();
        st << "{\"result\":\"" + _result + "\",\"data\":";
        if (data.size() != 0)
            Poco::JSON::Stringifier::condense(data, st);
        else
            st << "null";
        st << "}";
        st.flush();
        throw std::exception("JsonResponse sent");
    }
};

#define SALT_LENGTH 64

static std::string getPasswordHash(std::string password, std::string salt = genRandomString(SALT_LENGTH)) {
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> hs(salt, 4096);
    hs.update(password);
    return salt + ":" + hs.digestToHex(hs.digest());
}

static void Login(const RouteMatch& m) {
    std::string login, password;
    JsonRequest rq(m);
    rq.get("login", login);
    rq.get("password", password);
    
    auto user = DBConnection::instance().ExecParams("SELECT password, name FROM users WHERE login=$1", { login });
    if (user.row_count() != 1) JsonResponse(m, "BadCredentials").send();
    std::string passhash = (*user.begin()).field_by_name("password");
    if (getPasswordHash(password, passhash.substr(0, SALT_LENGTH)).compare(passhash))
        JsonResponse(m, "BadCredentials").send();
    std::string token = genRandomString();
    DBConnection::instance().ExecParams("UPDATE users SET token = $1 WHERE login = $2", { token, login });

    JsonResponse rs(m, "Ok");
    rs.set("name", (*user.begin()).field_by_name("name"));
    rs.set("accessToken", token);
    rs.send();
}

static void Logout(const RouteMatch& m) {
    std::string token;
    JsonRequest(m).get("accessToken", token);

    auto res = DBConnection::instance().ExecParams("SELECT token FROM users WHERE token=$1", { token });
    if (!res.row_count()) JsonResponse(m, "NotLoggedIn").send();
    DBConnection::instance().ExecParams("UPDATE users SET token = '' WHERE token = $1", { token });
    JsonResponse(m, "Ok").send();
}

static void Register(const RouteMatch& m) {
    std::string login, password, name;
    JsonRequest rq(m);
    rq.get("login", login);
    rq.get("password", password);
    rq.get("name", name);

    if (login.length() < 2 || login.length() > 36) JsonResponse(m, "BadLogin").send();
    if (DBConnection::instance().ExecParams("SELECT login FROM users WHERE login=$1", { login }).row_count() != 0) JsonResponse(m, "LoginExist").send();
    if (password.length() < 2 || password.length() > 36) JsonResponse(m, "BadPassword").send();
    if (name.length() < 2 || name.length() > 36) JsonResponse(m, "BadName").send();

    std::string token = genRandomString();
    DBConnection::instance().ExecParams("INSERT INTO users (login, name, password, token) values ($1, $2, $3, $4)", {
        login, name, getPasswordHash(password), token
    });

    JsonResponse rs(m, "Ok");
    rs.set("accessToken", token);
    rs.send();
}

template <void (*T)(const RouteMatch& )>
static void Wrapper(const RouteMatch& m) {
    try {
        T(m);
    }
    catch (...) {
        if (!m.response().sent())
            JsonResponse(m, "InternalError").send();
    }
}

class Users {
public:
    Users() {
        rnd.seed();
        auto & router = Router::instance();
        router.registerRoute("POST", "/api/login", Wrapper<Login>);
        router.registerRoute("POST", "/api/register", Wrapper<Register>);
        router.registerRoute("POST", "/api/logout", Wrapper<Logout>);
    }
};

static Users users;

