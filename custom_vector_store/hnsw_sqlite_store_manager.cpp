#include <vector>
#include <string>
#include <sqlite3.h>
#include <hnswlib.h>

#include "store_manager.hpp"
#include "hnsw_vec_store.cpp"
#include "sqlite_doc_store.cpp"

namespace simpleVec::manager
{

    class HNSWSqliteStoreManager : public simpleVec::manager::StoreManager<hnswlib::HierarchicalNSW<float>, hnswlib::SpaceInterface<float>, sqlite3>
    {

    public:
        HNSWSqliteStoreManager(int dim, const std::string &vector_store_path, const std::string &document_store_path,
                               const std::string &similarity_function, int M = 16, int ef_construction = 200, int max_elements = 1000)
        {

            this->dim = dim;
            this->vector_store = std::make_unique<simpleVec::store::HNSWVectorStore>(dim, vector_store_path, similarity_function, M, ef_construction, max_elements);
            this->document_store = std::make_unique<simpleVec::store::SQLiteDocStore>(document_store_path);
        }

        void add(int id, const std::vector<float> &vector, const std::string &document_content)
        {

            this->vector_store->add_vector(id, vector);
            this->document_store->add_document(id, document_content);
        }

        std::vector<std::pair<int, std::string>> search(const std::vector<float> &query_vector, int k, int ef = 100)
        {

            std::vector<int> vec_ids = this->vector_store->search(query_vector, k, ef);
            std::vector<std::pair<int, std::string>> documents = this->document_store->get_documents(vec_ids);
            return documents;
        }
    };

}
