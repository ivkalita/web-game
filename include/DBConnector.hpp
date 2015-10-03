#ifndef DBCONNECTOR_H_INCLUDED
#define DBCONNECTOR_H_INCLUDED

#include "libpq-fe.h"
#include "Poco/SingletonHolder.h"

#include <string>
#include <cstdlib>
#include <exception>
#include <initializer_list>
using namespace std;


class ConnectionException : public exception {
private:
    string message;
public:
    ConnectionException(string _message) : message(_message) {}

    const char* what() const override {
        return message.c_str();
    }
};


class DBConnection {
private:
    PGconn* conn;

    class QueryResult {
    private:
        PGresult* result;

        ExecStatusType get_status_type() const {
            PQresultStatus(result);
        }

        class Row {
        private:
            int row_num;
            QueryResult& parent;
        public:
            Row(QueryResult& _parent, int _row_num) : parent(_parent), row_num(_row_num) {}

            string field_by_name(string field_name) const {
                return parent.field_by_name(row_num, field_name);
            }

            string get(int col) const {
                return parent.get(row_num, col);
            }
        };

        class Iterator {
        private:
            int row_num;
            QueryResult& parent;
        public:
            Iterator(QueryResult& _parent, int _row_num) : parent(_parent), row_num(_row_num) {}

            int get_row_num() const {
                return row_num;
            }

            Iterator operator ++ () {
                row_num++;
                return *this;
            }

            Iterator operator -- () {
                row_num++;
                return *this;
            }

            bool operator == (const Iterator& a) {
                return a.get_row_num() == get_row_num();
            }

            bool operator != (const Iterator& a) {
                return a.get_row_num() != get_row_num();
            }
            
            Row operator * () {
                return Row(parent, row_num);
            }

        };

    public:
        QueryResult(PGresult* _result) : result(_result) {}

        ~QueryResult() {
            PQclear(result);
        }


        int row_count() const {
            return PQntuples(result);
        }

        int column_count() const {
            return PQnfields(result);
        }

        string column_name(int column_number) const {
            return PQfname(result, column_number);
        }

        string get(int row, int col) const {
            return PQgetvalue(result, row, col); //The caller should not free the result directly
        }

        string get_status_string() const {
            return PQresStatus(get_status_type());
        }
    
        string field_by_name(int row, string field_name) const {
            return get(row, PQfnumber(result, field_name.c_str()));
        }

        Iterator begin() {
            return Iterator(*this, 0);
        }

        Iterator end() {
            return Iterator(*this, row_count());
        }
    };

public:
    DBConnection(): conn(nullptr) {}

    ~DBConnection() {
        PQfinish(conn);
    }

    static DBConnection& instance() {
        static Poco::SingletonHolder<DBConnection> sh;
        return *sh.get();
    }


    void Connect(string hostaddr, string port, string dbname, string user, string password) {
        const char* const keywords[] = { "hostaddr", "port", "dbname", "user", "password", nullptr};
        const char* const values[] = { hostaddr.c_str(), port.c_str(), dbname.c_str(), user.c_str(), password.c_str(), nullptr};

        conn = PQconnectdbParams(keywords, values, 0);
        if (PQstatus(conn) == CONNECTION_BAD) {
            throw ConnectionException(string("Failed to connect to database. Message: '") + PQerrorMessage(conn) + "'");
        }

#if (_DEBUG)
        cout << "Connected to DB" << endl;
#endif
    }


    /*  Use $1, $2, etc in statement as a placeholder for parameters.
    *    Postgresql will infare parameters types. To explicitly set a parameter type use, for example,
    *    SELECT * FROM mytable WHERE x = $1::bigint;
    *    
    *    initializer_list is { "1", "2", "abc" } 
    */

    //template <class T>
    QueryResult ExecParams(string statement, initializer_list<string> params) {
        if (!conn)
            throw ConnectionException("connection is null");
        
        if (PQstatus(conn) == CONNECTION_BAD)
            throw ConnectionException("connection status is CONNECTION_BAD");

        char** values = nullptr;
        if (params.size() > 0) {
            values = new char*[params.size()];
            int i = 0;
            for (auto param : params) {
                values[i] = new char[param.length() + 1];
                strcpy(values[i], param.c_str());
                i++;
            }
        }
            
        PGresult* res = PQexecParams(conn, statement.c_str(), params.size(), nullptr, values, nullptr, nullptr, 0);
        return QueryResult(res);
    }
};


#endif
