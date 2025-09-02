# vdbms-playground

```bash
python3 -m venv venv
```

## Qdrant

### Local Build

Clone Qdrant repo at https://github.com/qdrant/qdrant.

1. Install rust and rustfmt.
```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
rustup install nightly
rustup override set nightly
rustup component add rustfmt
```
2. Install dependencies.
```bash
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y curl unzip gcc-multilib \
    clang cmake jq build-essential
```
3. Install protoc from source.
```bash
PROTOC_VERSION=22.2
PKG_NAME=$(uname -s | awk '{print ($1 == "Darwin") ? "osx-universal_binary" : (($1 == "Linux") ? "linux-x86_64" : "")}')

# curl `proto` source file
curl -LO https://github.com/protocolbuffers/protobuf/releases//download/v$PROTOC_VERSION/protoc-$PROTOC_VERSION-$PKG_NAME.zip

unzip protoc-$PROTOC_VERSION-$PKG_NAME.zip -d $HOME/.local

export PATH="$PATH:$HOME/.local/bin"

# remove source file if not needed
rm protoc-$PROTOC_VERSION-$PKG_NAME.zip

# check installed `protoc` version
protoc --version
```
4. Download the web UI resources.
```bash
./tools/sync-web-ui.sh
```
5. Build qdrant.
```bash
rustup install 1.87.0
rustup override set 1.87.0
module load llvm
cargo build --release --bin qdrant
```
6. Run qgrant server.
```bash
./target/release/qdrant
```
7. Run automated tests.
```bash
python3 -m venv venv
source venv/bin/activate
pip install poetry
poetry cache clear . --all
cd tests
poetry install
poetry sync
pip install poetry
poetry run pytest openapi
```
8. Access the web UI at `http://localhost:6333/dashboard`
9. Run a simple quickstart with (qdrant_test.py)[/qdrant/qdrant_test.py]
    - Refer - https://qdrant.tech/documentation/quickstart/
```bash
source venv/bin/activate
pip install qdrant-client
python3 qdrant_test.py
```

## PGVector

### Local Build

Clone repo at https://github.com/pgvector/pgvector

1. Install PostgreSQL 15
```bash
sudo apt install curl ca-certificates gnupg lsb-release
curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo gpg --dearmor -o /usr/share/keyrings/postgresql.gpg
echo "deb [signed-by=/usr/share/keyrings/postgresql.gpg] http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" | \
  sudo tee /etc/apt/sources.list.d/pgdg.list
sudo apt update
sudo apt install postgresql-15 postgresql-server-dev-15
source ~/.zshrc
```

2. Build PGVector extension.
```bash
make
sudo make PG_CONFIG=/usr/lib/postgresql/15/bin/pg_config install
```

3. Start PG Server and log in.
```bash
sudo pg_ctlcluster 15 main start
pg_lsclusters # check status
sudo -u postgres psql
```

4. Create a vector db.
```bash
https://github.com/pgvector/pgvector?tab=readme-ov-file#getting-started 
https://github.com/pgvector/pgvector?tab=readme-ov-file#hnsw
```

5. View created indexes.
```bash
\d items
```

6. Query with explanation.
```bash
EXPLAIN ANALYZE SELECT * FROM items ORDER BY embedding <-> '[3,1,2]' LIMIT 5;
```

7. Force indexing.
```bash
SET enable_seqscan = OFF;
```

## ClickHouse

```bash
git submodule update --init
rustup toolchain install nightly-2024-12-01
rustup default nightly-2024-12-01
rustup component add rust-src
module load cmake/3.30.2 llvm/nightly
export PATH=$HOME/local/yasm/bin:$PATH
export PATH=$HOME/local/nasm/bin:$PATH
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Debug ..
ninja clickhouse
```

## Custom Vector Store

```bash
cd custom_vector_store
git clone https://github.com/nmslib/hnswlib.git
mkdir build
cd build
cmake ..
make
```

## milvus-lite

### Local Build - NERSC Perlmutter

```bash
git clone https://github.com/milvus-io/milvus-lite.git
cd milvus-lite
git submodule update --init --recursive

module load python gcc-native/13 cmake cray-libsci
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip setuptools wheel build
pip install "conan<2.0,>=1.59"
conan profile new default --detect --force

export CC=$(which gcc)
export CXX=$(which g++)
export CXXFLAGS="-pthread -std=c++17"
export LDFLAGS="-pthread"

conan profile update settings.compiler=gcc default
conan profile update settings.compiler.version=13 default
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.compiler.cppstd=17 default

conan install . --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11

cmake . \
  -DCMAKE_BUILD_TYPE=Release \
  -DBLAS_LIBRARIES="-lsci_gnu_mp -lgfortran" \
  -DLAPACK_LIBRARIES="-lsci_gnu_mp -lgfortran"

cmake --build .

cd python
python3 -m build --wheel

```

> **Note:** Fix any header paths as necessary in `pb` foler.

### Local Build - WSL
```bash
git clone https://github.com/dhaura/milvus-lite.git
git submodule update --init --recursive
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip setuptools wheel build
pip install "conan<2.0,>=1.59"
chmod +x scripts/install_deps.sh
./scripts/install_deps.sh
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source "$HOME/.cargo/env"
cd python
python3 -m build --wheel
```

```bash
git clone https://github.com/dhaura/vdbms-playground.git
cd milvus-lite
python3 -m venv venv
source venv/bin/activate
pip install path/to/built/wheel/milvus_lite-2.5.2-py3-none-manylinux2014_x86_64.whl
pip install -U pymilvus
python3 ml_test.py
```
