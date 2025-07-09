#pragma once

#include <string>
#include <vector>
#include "vec_store.hpp"
#include "doc_store.hpp"

namespace simpleVec::manager
{

    template <typename INDEX_TYPE, typename DISTANCE_TYPE, typename STORE_TYPE>
    class StoreManager
    {

    public:
        virtual ~StoreManager() = default;
        virtual void add(int id, const std::vector<float> &vector, const std::string &document_content) = 0;
        virtual std::vector<std::pair<int, std::string>> search(const std::vector<float> &query_vector, int k, int ef) = 0;

    protected:
        int dim;
        std::unique_ptr<simpleVec::store::DocumentStore<STORE_TYPE>> document_store;
        std::unique_ptr<simpleVec::store::VectorStore<INDEX_TYPE, DISTANCE_TYPE>> vector_store;
    };

}
