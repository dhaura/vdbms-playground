#include <vector>
#include <string>
#include <stdexcept>
#include <sqlite3.h>

#include "doc_store.hpp"

namespace simpleVec::store
{

    class SQLiteDocStore : public simpleVec::store::DocumentStore<sqlite3>
    {

    public:
        SQLiteDocStore(const std::string &db_path)
        {

            this->db_path = db_path;
            if (sqlite3_open(db_path.c_str(), &this->store) != SQLITE_OK)
            {
                throw std::runtime_error("Failed to open database: " + db_path);
            }

            initialize();
        }

        ~SQLiteDocStore()
        {

            sqlite3_close(this->store);
        }

        void add_document(int id, const std::string &content) override
        {

            std::string sql = "INSERT OR REPLACE INTO documents (id, content) VALUES (?, ?)";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(this->store, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
                throw std::runtime_error("Prepare insert failed");

            sqlite3_bind_int(stmt, 1, id);
            sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) != SQLITE_DONE)
                throw std::runtime_error("Insert failed");

            sqlite3_finalize(stmt);
        }

        std::vector<std::pair<int, std::string>> get_documents(const std::vector<int> &ids) override
        {

            std::vector<std::pair<int, std::string>> documents;
            for (int id : ids)
            {
                std::string sql = "SELECT content FROM documents WHERE id = ?";
                sqlite3_stmt *stmt;
                if (sqlite3_prepare_v2(this->store, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
                    throw std::runtime_error("Prepare select failed");

                sqlite3_bind_int(stmt, 1, id);

                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    const char *content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
                    documents.emplace_back(id, content ? content : "");
                }

                sqlite3_finalize(stmt);
            }
            return documents;
        }

    private:
        void initialize()
        {

            std::string sql = "CREATE TABLE IF NOT EXISTS documents (id INTEGER PRIMARY KEY, content TEXT);";

            char *errMsg = nullptr;
            if (sqlite3_exec(this->store, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
            {
                std::string msg = errMsg;
                sqlite3_free(errMsg);
                throw std::runtime_error("Failed to create table: " + msg);
            }
        }
    };

}
