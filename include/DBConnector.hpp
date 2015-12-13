#ifndef DBCONNECTOR_H_INCLUDED
#define DBCONNECTOR_H_INCLUDED

#include "libpq-fe.h"
#include "Poco/SingletonHolder.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <initializer_list>
#include <memory>


class ConnectionException : public std::exception {
private:
    std::string message;
public:
    ConnectionException(std::string _message) : message(_message) {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
};


class DBConnection {
private:
    PGconn* conn;
    int prepared_statement_counter;

    static void result_deleter(PGresult* result) {
        PQclear(result);
    }

    void check_errors(PGresult* result) {
        std::string error_message = PQresultErrorMessage(result);
        if (!error_message.empty()) {
            throw ConnectionException(error_message);
        }
    }
    
    void check_connection() {
        if (!conn)
            throw ConnectionException("connection is null (disconnected)");
            
        if (PQstatus(conn) == CONNECTION_BAD)
            throw ConnectionException("connection status is CONNECTION_BAD");
    }

    char** prepare_values_array(std::initializer_list<std::string>& params) {
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

        return values;
    }

    void free_values_array(char** values, int size) {
        for (int i = 0; i < size; i++)
            delete values[i];
        delete values;
    }

    class QueryResult;

    class PreparedStatement {
    private:
        DBConnection* parent;
        std::string query, stmt_name;
        std::shared_ptr<PGresult> res;

    public:
        PreparedStatement(DBConnection* _parent, std::string _query) : parent(_parent), query(_query) {
            parent->check_connection();
            stmt_name = "stmt" + std::to_string(parent->prepared_statement_counter++);

            res = std::shared_ptr<PGresult>(PQprepare(
                parent->conn,
                stmt_name.c_str(),
                query.c_str(),
                0,
                nullptr), result_deleter);

            parent->check_errors(res.get());
        }

        PreparedStatement(const PreparedStatement& a): parent(a.parent), query(a.query), stmt_name(a.stmt_name) {
            res = a.res;
        }

        PreparedStatement operator = (const PreparedStatement& a) {
            parent = a.parent;
            query = a.query;
            stmt_name = a.stmt_name;
            res = a.res;
            return *this;
        }

        QueryResult Exec(std::initializer_list<std::string> params) {
            parent->check_connection();

            char** values = parent->prepare_values_array(params);

            PGresult* result = PQexecPrepared(
                parent->conn,
                stmt_name.c_str(),
                params.size(),
                values,
                nullptr,
                nullptr,
                0);

            parent->free_values_array(values, params.size());

            parent->check_errors(result);

            return QueryResult(result);
        }
        
    };

    friend class PreparedStatement;

    class QueryResult {
    private:
        std::shared_ptr<PGresult> result;

        ExecStatusType get_status_type() const {
            return PQresultStatus(result.get());
        }

        class Row {
        private:
            QueryResult& parent;
            int row_num;
        public:
            Row(QueryResult& _parent, int _row_num) : parent(_parent), row_num(_row_num) {}

            std::string field_by_name(std::string field_name) const {
                return parent.field_by_name(row_num, field_name);
            }

            std::string get(int col) const {
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
                ++row_num;
                return *this;
            }

            Iterator operator -- () {
                --row_num;
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
        QueryResult(PGresult* _result) : result(_result, result_deleter) {}

        QueryResult(const QueryResult& a) {
            result = a.result;
        }

        QueryResult operator = (const QueryResult& a) {
            result = a.result;
            return *this;
        }

        int row_count() const {
            return PQntuples(result.get());
        }

        int column_count() const {
            return PQnfields(result.get());
        }

        std::string column_name(int column_number) const {
            return PQfname(result.get(), column_number);
        }

        std::string get(int row, int col) const {
            return PQgetvalue(result.get(), row, col);
        }

        std::string get_status_string() const {
            return PQresStatus(get_status_type());
        }
    
        std::string field_by_name(int row, std::string field_name) const {
            return get(row, PQfnumber(result.get(), field_name.c_str()));
        }

        Iterator begin() {
            return Iterator(*this, 0);
        }

        Iterator end() {
            return Iterator(*this, row_count());
        }
    };

    DBConnection(const DBConnection&) {}
    DBConnection& operator = (const DBConnection&) { return *this; }

public:
    DBConnection(): conn(nullptr), prepared_statement_counter(0) {}

    void Disconnect() {
        if (conn) {
            PQfinish(conn);
            conn = nullptr;
        }
    };

    ~DBConnection() {
        Disconnect();
    }

    static DBConnection& instance() {
        static Poco::SingletonHolder<DBConnection> sh;
        return *sh.get();
    }

    PreparedStatement Prepare(std::string query) {
        return PreparedStatement(this, query);
    }
    
    void Connect(std::string hostaddr, std::string port, std::string dbname, std::string user, std::string password) {
        const char* const keywords[] = { "hostaddr", "port", "dbname", "user", "password", nullptr};
        const char* const values[] = { hostaddr.c_str(), port.c_str(), dbname.c_str(), user.c_str(), password.c_str(), nullptr};

        conn = PQconnectdbParams(keywords, values, 0);
        if (PQstatus(conn) == CONNECTION_BAD) {
            throw ConnectionException(std::string("Failed to connect to database. Message: '") + PQerrorMessage(conn) + "'");
        }

#if (_DEBUG)
        cout << "Connected to DB" << endl;
#endif
    }


    /*  Use $1, $2, etc in statement as a placeholder for parameters.
    *    Postgresql will infer parameters types. To explicitly set a parameter type use, for example,
    *    SELECT * FROM mytable WHERE x = $1::bigint;
    *    
    *    initializer_list is { "1", "2", "abc" } 
    */

    QueryResult ExecParams(std::string statement, std::initializer_list<std::string> params) {
        check_connection();

        char** values = prepare_values_array(params);

        PGresult* res = PQexecParams(
            conn,
            statement.c_str(),
            params.size(),
            nullptr,
            values,
            nullptr,
            nullptr,
            0);

        free_values_array(values, params.size());

        check_errors(res);
        return QueryResult(res);
    }
};


#endif
