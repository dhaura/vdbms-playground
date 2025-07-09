#include <vector>
#include <string>
#include <cstring>
#include <fstream>

#include "hnsw_sqlite_store_manager.cpp"

int main(int argc, char **argv)
{

    int dim = 0;
    std::string index_path = "";
    std::string doc_path = "";
    std::string similarity_function = "";

    int k = 0;
    int num_docs = 0;
    int num_queries = 0;
    std::string input_doc_file = "";
    std::string input_doc_embed_file = "";
    std::string query_file = "";
    std::string query_embed_file = "";
    std::string gt_file = "";

     for (int p = 0; p < argc; p++)
    {
        if (strcmp(argv[p], "-index_path") == 0)
        {
            index_path = argv[p + 1];
        }
        else if (strcmp(argv[p], "-doc_path") == 0)
        {
            doc_path = argv[p + 1];
        }
        else if (strcmp(argv[p], "-sf") == 0)
        {
            similarity_function = argv[p + 1];
        }
        else if (strcmp(argv[p], "-dim") == 0)
        {
            dim = atoi(argv[p + 1]);
        }
        else if (strcmp(argv[p], "-k") == 0)
        {
            k = atoi(argv[p + 1]);
        }
        else if (strcmp(argv[p], "-num_docs") == 0)
        {
            num_docs = atoi(argv[p + 1]);
        }
        else if (strcmp(argv[p], "-num_queries") == 0)
        {
            num_queries = atoi(argv[p + 1]);
        }
        else if (strcmp(argv[p], "-input_doc_file") == 0)
        {
            input_doc_file = argv[p + 1];
        }
        else if (strcmp(argv[p], "-input_doc_embed_file") == 0)
        {
            input_doc_embed_file = argv[p + 1];
        }
        else if (strcmp(argv[p], "-query_file") == 0)
        {
            query_file = argv[p + 1];
        }
        else if (strcmp(argv[p], "-query_embed_file") == 0)
        {
            query_embed_file = argv[p + 1];
        }
        else if (strcmp(argv[p], "-gt_file") == 0)
        {
            gt_file = argv[p + 1];
        } 
    }

    simpleVec::manager::HNSWSqliteStoreManager store_manager(dim, index_path, doc_path, similarity_function);

    std::cout << "HNSW SQLite Store Manager initialized." << std::endl;

    // Add vectors.
    std::ifstream doc_file(input_doc_file);
    std::ifstream doc_embed_file(input_doc_embed_file, std::ios::binary);
    std::string line;
    int id = 0;

    std::vector<float> doc_vectors(num_docs * dim);
    doc_embed_file.read(reinterpret_cast<char*>(doc_vectors.data()), doc_vectors.size() * sizeof(float));
    
    while (std::getline(doc_file, line))
    {
        std::vector<float> vector(dim);
        std::memcpy(vector.data(), &doc_vectors[id * dim], dim * sizeof(float));
        std::string content = line;

        store_manager.add(id, vector, content);
        id++;
    }

    doc_file.close();
    doc_embed_file.close();

    std::cout << "Added " << num_docs << " docs and their embedding vectors to the store.\n" << std::endl;

    // Search for nearest neighbors.
    std::ifstream query_file_stream(query_file);
    std::ifstream query_embed_file_stream(query_embed_file, std::ios::binary);
    std::ifstream gt_file_stream(gt_file, std::ios::binary);

    std::vector<float> query_vectors(num_queries * dim);
    query_embed_file_stream.read(reinterpret_cast<char*>(query_vectors.data()), query_vectors.size() * sizeof(float));

    std::vector<int> gt_ids(num_queries);
    gt_file_stream.read(reinterpret_cast<char*>(gt_ids.data()), num_queries * sizeof(int));

    float num_correct = 0.0f;
    int id_query = 0;
    while (std::getline(query_file_stream, line))
    {
        std::vector<float> query_vector(dim);
        std::memcpy(query_vector.data(), &query_vectors[id_query * dim], dim * sizeof(float));

        std::string query_content = line;
        std::vector<std::pair<int, std::string>> results = store_manager.search(query_vector, k);

        std::cout << "Ground Truth ID: " << gt_ids[id_query] << std::endl;
        std::cout << "Query: " << query_content << std::endl;
        std::cout << "Top " << k << " results:" << std::endl;
        for (const auto &result : results)
        {               
            std::cout << "ID: " << result.first << ", Content: " << result.second << std::endl;
        }
        std::cout << std::endl;

        for (const auto &result : results)
        {
            if (result.first == gt_ids[id_query])
            {
                num_correct += 1.0f;
                break;
            }
        }
        id_query++;
    }

    query_file_stream.close();
    query_embed_file_stream.close();
    gt_file_stream.close();

    float recall = num_correct / num_queries;
    std::cout << "Final Recall@K: " << recall << std::endl;

    return 0;
}
