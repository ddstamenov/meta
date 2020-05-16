# Build tests

cd tests

### build all tests
./make.sh

### buid with clang-static-analyzer
scan-build-3.8 -o build/report ./make.sh
