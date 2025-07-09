import numpy as np
import torch
from sentence_transformers import SentenceTransformer
from sklearn.metrics.pairwise import cosine_similarity
import os

docs = [
    "Cats are great pets and love to purr.",
    "The universe is vast and full of stars.",
    "Quantum computing is a new paradigm.",
    "Pasta is a popular Italian dish.",
    "Vaccines help prevent diseases."
]

queries = [
    "What do cats do when happy?",
    "What is quantum computing?",
    "How to make Italian food?"
]

output_dir = "/home/dhaura/repos/vdbms-playground/custom_vector_store/test/data"
embedding_model = "all-MiniLM-L6-v2"
top_k = 1
# Load SBERT model
print(f"Loading model {embedding_model}...")
model = SentenceTransformer(embedding_model, device='cuda' if torch.cuda.is_available() else 'cpu')
print("Model loaded successfully.")

# Encode documents and queries
print("Encoding documents and queries...")
doc_embs = model.encode(docs, normalize_embeddings=True)
query_embs = model.encode(queries, normalize_embeddings=True)
print("Encoding completed.")

# Add ground truth neighbors.
manual_qrels = {
    0: [0],      # "What do cats do when happy?" → "Cats are great pets..."
    1: [2],      # "What is quantum computing?" → "Quantum computing..."
    2: [3]       # "How to make Italian food?" → "Pasta is a popular Italian dish."
}
num_queries = len(manual_qrels)
top_k = 1

gt = np.full((num_queries, top_k), fill_value=-1, dtype=np.int32)

for qid in range(num_queries):
    relevant_docs = manual_qrels.get(qid, [])
    for k in range(min(top_k, len(relevant_docs))):
        gt[qid][k] = relevant_docs[k]

# Save files
os.makedirs(output_dir, exist_ok=True)

# Save docs and queries
print("Saving documents and queries...")
with open(f"{output_dir}/docs.txt", "w") as f:
    for doc in docs:
        f.write(doc.strip().replace("\n", " ") + "\n")

with open(f"{output_dir}/queries.txt", "w") as f:
    for query in queries:
        f.write(query.strip().replace("\n", " ") + "\n")
print("Documents and queries saved.")

# Save embeddings
print("Saving embeddings...")
doc_embs.tofile(f"{output_dir}/doc_embeddings.bin")
query_embs.tofile(f"{output_dir}/query_embeddings.bin")
gt.tofile(f"{output_dir}/ground_truth.bin")
print("Embeddings saved.")

print(f"Saved testset in {output_dir}")
