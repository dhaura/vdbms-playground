cmake_minimum_required(VERSION 3.12)
project(HNSW_SQLITE_VecDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include HNSWlib headers.
include_directories($ENV{HNSWLIB_ROOT}/hnswlib)

link_directories(
    $ENV{HNSWLIB_ROOT}/hnswlib
)


# Find SQLite3.
find_package(SQLite3 REQUIRED)

add_executable(test_hnswlib_sqlite_vec_db
    test_hnswlib_sqlite_vec_db.cpp
    hnsw_vec_store.cpp
    sqlite_doc_store.cpp
    hnsw_sqlite_store_manager.cpp
)

# Link SQLite3.
target_link_libraries(test_hnswlib_sqlite_vec_db PRIVATE SQLite::SQLite3)
