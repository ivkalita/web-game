#include "DBConnector.hpp"
#include "WebgameServer.hpp"
#include "Poco/Util/IniFileConfiguration.h"
#include <gtest/gtest.h>

using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

class QueryTest : public ::testing::Test {
public:
    AutoPtr<IniFileConfiguration> c;

    virtual void SetUp() {
        //WebgameServer::instance().loadConfiguration();
        //auto c = &WebgameServer::instance().config();
        c = new IniFileConfiguration("bin/tester.ini");
      
        DBConnection::instance().Connect(
            c->getString("tester.hostaddr"),
            c->getString("tester.port"),
            c->getString("tester.dbname"),
            c->getString("tester.user"),
            c->getString("tester.password"));
    }

    virtual void TearDown() {
        DBConnection::instance().Disconnect();
    }
};


TEST(SingletonTest, Equal) {
    ASSERT_EQ(&DBConnection::instance(), &DBConnection::instance()) << "instance is the same address";
}


TEST_F(QueryTest, SelectInserted) {
   
    char* pl_name = "test'player";
    DBConnection::instance().ExecParams("DELETE FROM players where login=$1", { pl_name });
    DBConnection::instance().ExecParams("INSERT INTO players (login, password) values ($1, $2)", { pl_name, "pswd" });
    auto res = DBConnection::instance().ExecParams("SELECT * FROM players where login=$1", { pl_name });
    for (auto i = res.begin(); i != res.end(); ++i) {
        ASSERT_EQ((*i).field_by_name("login"), string(pl_name));
    }
}

TEST_F(QueryTest, PreparedStmt) {
    char* names[3][2] = { { "login1", "pwd1" }, { "login2", "pwd2" }, { "login3", "pwd3" } };

    DBConnection::instance().ExecParams("DELETE FROM players", {});

    auto ps = DBConnection::instance().Prepare("INSERT INTO players (login, password) values ($1, $2)");

    for (int i = 0; i < 3; i++) {
        ps.Exec({ names[i][0], names[i][1] });
    }

    auto res = DBConnection::instance().ExecParams("SELECT * FROM players ORDER BY login", {});
    int j = 0;
    for (auto i = res.begin(); i != res.end(); ++i) {
        EXPECT_EQ((*i).field_by_name("login"), string(names[j++][0]));
    }

}
