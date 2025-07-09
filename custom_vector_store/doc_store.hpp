#pragma once

#include <vector>
#include <string>

namespace simpleVec::store
{

    template <typename STORE_TYPE>
    class DocumentStore
    {

    public:
        virtual ~DocumentStore() = default;
        virtual void add_document(int id, const std::string &content) = 0;
        virtual std::vector<std::pair<int, std::string>> get_documents(const std::vector<int> &ids) = 0;

    protected:
        std::string db_path;
        STORE_TYPE *store;
    };

}
