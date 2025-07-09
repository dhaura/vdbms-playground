#pragma once

#include <vector>
#include <string>

namespace simpleVec::store
{

    template <typename INDEX_TYPE, typename DISTANCE_TYPE>
    class VectorStore
    {

    public:
        virtual ~VectorStore() = default;
        virtual void add_vector(int id, const std::vector<float> &vector) = 0;
        virtual std::vector<int> search(const std::vector<float> &query_vector, int k, int ef) = 0;
        virtual void save_index() = 0;
        virtual void load_index() = 0;

    protected:
        int dim;
        std::string index_path;
        std::unique_ptr<INDEX_TYPE> index;
        std::unique_ptr<DISTANCE_TYPE> space;
    };

}
