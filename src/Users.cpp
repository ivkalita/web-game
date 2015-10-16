#include "Users.h"
#include "Pages.h"
#include "DBConnector.hpp"

#include "Poco/JSON/Template.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include "Poco/JSON/Query.h"

#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NameValueCollection.h"

#include "Poco/Net/HTTPResponse.h"

#include "Poco/Net/HTTPCookie.h"

#include "Poco/Random.h"
#include "Poco/SHA1Engine.h"


using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

void AddUserInfo(HTTPServerRequest& request, Poco::JSON::Object::Ptr params) {
	Poco::Net::NameValueCollection cookies;
	request.getCookies(cookies);
	if (!cookies.has("token") || !cookies.get("token").length()) return;
	auto res = DBConnection::instance().ExecParams("SELECT * FROM users WHERE token=$1", { cookies.get("token") });
	if (res.begin() != res.end()) {
		Poco::JSON::Object::Ptr user(new Poco::JSON::Object);
		user->set("email", (*res.begin()).field_by_name("email"));
		user->set("name", (*res.begin()).field_by_name("name"));
		params->set("user", user);
	}
}

static std::string getRandomString() {
	Poco::Random rnd;
	rnd.seed();
	std::string token;
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < 32; i++)
		token += alphanum[rnd.next() % (sizeof(alphanum)-1)];
	return token;
}

static std::string getPasswordHash(std::string password, std::string salt = getRandomString()) {
	salt.resize(32);
	Poco::SHA1Engine hs;
	hs.reset();
	hs.update(password);
	hs.update(salt);
	return salt + ":" + hs.digestToHex(hs.digest());
}

static bool check(Poco::JSON::Object::Ptr params, Poco::Net::HTMLForm & form, const std::string & name, const std::string & message, bool & valid) {
	if (!form.has(name) || !form.get(name).length()) {
		valid = false;
		AddMessage(params, "danger", message);
	}
	return form.has(name) && form.get(name).length();
}

static std::string getParamString(Poco::JSON::Object::Ptr params, std::string query) {
	Poco::JSON::Query q(params);
	std::string _query(query), st;
	return q.findValue(_query, st);
}

static bool checkPassword(Poco::JSON::Object::Ptr params, const std::string & email, const std::string & password, const std::string & message) {
	auto res = DBConnection::instance().ExecParams("SELECT password FROM users WHERE email=$1", { email });
	bool valid = res.begin() != res.end();
	if (valid) {
		std::string passhash((*res.begin()).field_by_name("password"));
		valid &= !getPasswordHash(password, passhash).compare(passhash);
	}
	if (!valid) AddMessage(params, "danger", message);
	return valid;
}

static bool checkEmailExist(Poco::JSON::Object::Ptr params, const std::string & email, const std::string & message) {
	auto res = DBConnection::instance().ExecParams("SELECT email FROM users WHERE email=$1", { email });
	bool exist = res.begin() != res.end();
	if (exist) AddMessage(params, "danger", message);
	return exist;
}

namespace Pages {

	void Login(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		if (request.getMethod() == "POST") {
			Poco::Net::HTMLForm form(request, request.stream());
			bool valid = true;
			if (check(params, form, "email", "Email не указан.", valid)) {
				params->set("email", form.get("email"));
				if (check(params, form, "password", "Пароль не указан.", valid)) {
					valid &= checkPassword(params, form.get("email"), form.get("password"), "Неверные учётные данные. Проверьте email и пароль.");
					if (valid) {
						std::string token(getRandomString());
						DBConnection::instance().ExecParams("UPDATE users SET token = $1 WHERE email = $2", { token, form.get("email") });
						response.addCookie(Poco::Net::HTTPCookie("token", token));
						response.redirect("/", Poco::Net::HTTPResponse::HTTP_FOUND);
						return;
					}
				}
			}
		}
		params->set("title", "Вход в систему");
		TemplateRender(response, params, "login.html");
	}

	void Logout(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		response.addCookie(Poco::Net::HTTPCookie("token", ""));
		response.redirect("/", Poco::Net::HTTPResponse::HTTP_FOUND);
	}

	void Register(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		if (request.getMethod() == "POST") {
			Poco::Net::HTMLForm form(request, request.stream());
			bool valid = true;
			if (check(params, form, "email", "Email не указан.", valid)) {
				params->set("email", form.get("email"));
				valid &= !checkEmailExist(params, form.get("email"), "Пользователь с таким email уже существует.");
			}
			if (check(params, form, "name", "Имя не указано.", valid))
				params->set("name", form.get("name"));
			if (
				check(params, form, "password", "Пароль не указан.", valid)
				&& check(params, form, "password2", "Повтор пароля не указан.", valid)
				&& form.get("password").compare(form.get("password2"))
			) {
				valid = false;
				AddMessage(params, "danger", "Пароли не совпадают.");
			}
			if (valid) {
				std::string token(getRandomString());
				DBConnection::instance().ExecParams("INSERT INTO users (email, name, password, token) values ($1, $2, $3, $4)",
					{ form.get("email"), form.get("name"), getPasswordHash(form.get("password")), token });
				response.addCookie(Poco::Net::HTTPCookie("token", token));
				response.redirect("/", Poco::Net::HTTPResponse::HTTP_FOUND);
				return;
			}
		}
		params->set("title", "Регистрация");
		TemplateRender(response, params, "register.html");
	}

	void Profile(HTTPServerRequest& request, HTTPServerResponse& response, Poco::JSON::Object::Ptr params) {
		if (!params->has("user")) throw Poco::FileAccessDeniedException();
		if (request.getMethod() == "POST") {
			Poco::Net::HTMLForm form(request, request.stream());
			bool valid = true;
			std::string email = getParamString(params, "user.email");
			check(params, form, "change", "Метод не указан.", valid);
			if (valid && !form.get("change").compare("common")) {
				check(params, form, "name", "Имя не указано.", valid);
				if (valid) {
					DBConnection::instance().ExecParams("UPDATE users SET name = $1 WHERE email = $2", { form.get("name"), email });
					response.redirect("/profile", Poco::Net::HTTPResponse::HTTP_FOUND);
					return;
				}
			}
			else if (valid && !form.get("change").compare("email")) {
				if (check(params, form, "password", "Пароль не указан.", valid))
					valid &= checkPassword(params, email, form.get("password"), "Неверные учётные данные. Проверьте пароль.");
				if (check(params, form, "email", "Email не указан.", valid))
					valid &= !checkEmailExist(params, form.get("email"), "Пользователь с таким email уже существует.");
				if (valid) {
					DBConnection::instance().ExecParams("UPDATE users SET email = $1 WHERE email = $2", { form.get("email"), email });
					response.redirect("/profile", Poco::Net::HTTPResponse::HTTP_FOUND);
					return;
				}
			}
			else if (valid && !form.get("change").compare("password")) {
				if (check(params, form, "password_current", "Пароль не указан.", valid))
					valid &= checkPassword(params, email, form.get("password_current"), "Неверные учётные данные. Проверьте пароль.");
				if (
					check(params, form, "password", "Новый пароль не указан.", valid)
					&& check(params, form, "password2", "Повтор пароля не указан.", valid)
					&& form.get("password").compare(form.get("password2"))
					) {
					valid = false;
					AddMessage(params, "danger", "Пароли не совпадают.");
				}
				if (valid) {
					DBConnection::instance().ExecParams("UPDATE users SET password = $1 WHERE email = $2",
					{ getPasswordHash(form.get("password")), email });
					response.redirect("/profile", Poco::Net::HTTPResponse::HTTP_FOUND);
					return;
				}
			}
		}
		params->set("title", "Профиль");
		TemplateRender(response, params, "profile.html");
	}
}