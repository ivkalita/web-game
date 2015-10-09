#include "DBConnector.hpp"
#include <gtest/gtest.h>

TEST(SingletonTest, Equal) {
    ASSERT_EQ(&DBConnection::instance(), &DBConnection::instance()) << "instance is the same address";
}

const char* password = "qwe"; // specify your password
const char* dbname = "galcon"; // specify your database name

TEST(QueryTest, SelectInserted) {
    DBConnection inst = DBConnection::instance();
    inst.Connect("127.0.0.1", "5432", dbname, "postgres", password);
    char* pl_name = "test'player";
    inst.ExecParams("DELETE FROM players where login=$1", { pl_name });
    inst.ExecParams("INSERT INTO players (login, password) values ($1, $2)", { pl_name, "pswd" });
    auto res = inst.ExecParams("SELECT * FROM players where login=$1", { pl_name });
    for (auto i = res.begin(); i != res.end(); ++i) {
        ASSERT_EQ((*i).field_by_name("login"), string(pl_name));
    }
    inst.Disconnect();
}

TEST(QueryTest, PreparedStmt) {
    DBConnection inst = DBConnection::instance();
    inst.Connect("127.0.0.1", "5432", dbname, "postgres", password);

    char* names[3][2] = { { "login1", "pwd1" }, { "login2", "pwd2" }, { "login3", "pwd3" } };

    inst.ExecParams("DELETE FROM players", {});

    auto ps = inst.CreatePreparedStatement("INSERT INTO players (login, password) values ($1, $2)");

    for (int i = 0; i < 3; i++) {
        ps.Exec({ names[i][0], names[i][1] });
    }

    auto res = inst.ExecParams("SELECT * FROM players ORDER BY login", {});
    int j = 0;
    for (auto i = res.begin(); i != res.end(); ++i) {
        ASSERT_EQ((*i).field_by_name("login"), string(names[j++][0]));
    }
    inst.Disconnect();

}
