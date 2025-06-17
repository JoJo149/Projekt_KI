FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    ninja-build \
    make \
    findutils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy all necessary source files and directories:
COPY CMakeLists.txt .
COPY clientsrc/ /app/clientsrc/
COPY src/ /app/src/
COPY include/ /app/include/
COPY ext/ /app/ext/
COPY transposition_table/ /app/transposition_table/
COPY tests/ /app/tests/
COPY benchmark/ /app/benchmark/

# Remove old build if any, then create clean build folder
RUN rm -rf build && mkdir build

# Configure and build
RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \
    -DCMAKE_CXX_FLAGS="-std=c++20 -O3 -march=native -flto=auto -DNDEBUG" \
 && cmake --build build --target dockerClient

WORKDIR /app/build

USER nobody

CMD ["./dockerClient"]
