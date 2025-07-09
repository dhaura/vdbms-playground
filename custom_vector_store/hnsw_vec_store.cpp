#include <vector>
#include <string>
#include <filesystem>
#include <hnswlib.h>

#include "vec_store.hpp"

namespace simpleVec::store
{

    class HNSWVectorStore : public VectorStore<hnswlib::HierarchicalNSW<float>, hnswlib::SpaceInterface<float>>
    {

    public:
        HNSWVectorStore(int dim, const std::string &index_path, const std::string &similarity_function,
                        int M = 16, int ef_construction = 200, int max_elements = 1000)
        {

            this->dim = dim;
            this->index_path = index_path;
            this->M = M;
            this->ef_construction = ef_construction;
            this->max_elements = max_elements;

            if (similarity_function == "l2")
            {
                this->space = std::make_unique<hnswlib::L2Space>(dim);
            }
            else if (similarity_function == "ip")
            {
                this->space = std::make_unique<hnswlib::InnerProductSpace>(dim);
            }
            else
            {
                throw std::invalid_argument("Unsupported similarity function: " + similarity_function);
            }

            this->index = std::make_unique<hnswlib::HierarchicalNSW<float>>(this->space.get(), this->max_elements, this->M, this->ef_construction);
            this->load_index();
        }

        ~HNSWVectorStore()
        {
            this->save_index();
        }

        void load_index()
        {

            if (std::filesystem::exists(index_path))
            {
                try
                {
                    this->index->loadIndex(this->index_path, this->space.get());
                }
                catch (const std::exception &e)
                {
                    throw std::runtime_error("Failed to load index: " + std::string(e.what()));
                }
            }
            else
            {
                this->save_index();
            }
        }

        void save_index()
        {

            try
            {
                this->index->saveIndex(this->index_path);
            }
            catch (const std::exception &e)
            {
                throw std::runtime_error("Failed to save index: " + std::string(e.what()));
            }
        }

        void add_vector(int id, const std::vector<float> &vector)
        {

            if (vector.size() != this->dim)
            {
                throw std::invalid_argument("Vector dimension does not match store dimension.");
            }
            this->index->addPoint(vector.data(), id);
        }

        std::vector<int> search(const std::vector<float> &query_vector, int k, int ef = 100)
        {

            if (query_vector.size() != this->dim)
            {
                throw std::invalid_argument("Query vector dimension does not match store dimension.");
            }

            this->index->setEf(ef);
            auto results = this->index->searchKnn(query_vector.data(), k);

            std::vector<int> ids;
            ids.reserve(results.size());

            int current_id;
            while (!results.empty())
            {
                current_id = results.top().second;
                ids.push_back(current_id);
                results.pop();
            }

            return ids;
        }

    private:
        int M;
        int ef_construction;
        int max_elements;
    };

}
