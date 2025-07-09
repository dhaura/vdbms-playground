#!/bin/bash
#SBATCH --qos=debug
#SBATCH --time=30:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --constraint=cpu
#SBATCH --output=logs/%j.log

export OMP_NUM_THREADS=1

export HNSWLIB_ROOT=$SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/hnswlib

srun -n 1 build/test_hnswlib_sqlite_vec_db \
    -index_path $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/hnswlib_index.bin \
    -doc_path $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/documents.db \
    -sf ip \
    -dim 384 \
    -k 3 \
    -num_docs 5 \
    -num_queries 3 \
    -input_doc_file $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/docs.txt \
    -input_doc_embed_file $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/doc_embeddings.bin \
    -query_file $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/queries.txt \
    -query_embed_file $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/query_embeddings.bin \
    -gt_file $SCRATCH/repos/vectorDBs/vdbms-playground/custom_vector_store/test/data/ground_truth.bin
