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

output_dir = "vdbms-playground/custom_vector_store/test/data"
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

# Compute top-k ground truth neighbors
print("Computing cosine similarity...")
sims = cosine_similarity(query_embs, doc_embs)
gt = np.argsort(-sims, axis=1)[:, :top_k]  # shape: (num_queries, top_k)
print("Cosine similarity computed.")

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
