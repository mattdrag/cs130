#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "sql_engine.hpp"


class SqlEngineTest : public ::testing::Test {
protected:
    std::string HandleQuery(std::string query) {
        return handler.HandleRequest(query, 1);
    }
    std::string HandleUpdate(std::string update) {
        return handler.HandleRequest(update, 2);
    }
    void create(){
        handler.HandleRequest(createTable, 2);
    }
    void destroy(){
        handler.HandleRequest(dropTable, 2);
    }
    SqlEngine handler;
    std::string dropTable = "DROP TABLE IF EXISTS Test";
    std::string createTable = "CREATE TABLE Test(name VARCHAR(20))";
};

TEST_F(SqlEngineTest, QueryTest) {
    create();
    HandleUpdate("INSERT INTO Test(name) VALUES(\"test\")");
    std::string queryString = "SELECT * FROM Test";
    std::string expectedOutput = "<table><tr><th>name</th></tr><tr><td>test</td></tr></table>";
	EXPECT_EQ(expectedOutput, HandleQuery(queryString));
    destroy();
}

TEST_F(SqlEngineTest, EmptyQuery) {
    std::string errorString = "MYSQL ERR: INVALID SYNTAX";
    EXPECT_EQ(errorString, HandleQuery(""));
}

TEST_F(SqlEngineTest, UpdateTest) {
    create();
    std::string updateString = "INSERT INTO Test(name) VALUES(\"test\")";
    std::string expectedOutput = "Update Success!";
    EXPECT_EQ(expectedOutput, HandleUpdate(updateString));
    destroy();
}

TEST_F(SqlEngineTest, EmptyUpdate) {
    std::string errorString = "MYSQL ERR: INVALID SYNTAX";
    EXPECT_EQ(errorString, HandleUpdate(""));
}