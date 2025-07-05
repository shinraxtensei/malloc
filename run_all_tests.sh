#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Running All Malloc Tests ===${NC}"
echo

# Set up environment for our malloc library
export DYLD_LIBRARY_PATH=.
export DYLD_INSERT_LIBRARIES="./libft_malloc.so"
export DYLD_FORCE_FLAT_NAMESPACE=1

# Change to project directory
cd /Users/anasshouari/sideProjects/malloc

# Test function
run_test() {
    local test_name=$1
    local test_file=$2
    
    echo -e "${BLUE}--- Test $test_name ---${NC}"
    echo "Compiling $test_file..."
    
    # Add malloc declaration for test1 if needed
    if [ "$test_file" = "tests/test1.c" ]; then
        sed '1i\
#include <stdlib.h>' tests/test1.c > /tmp/test1_fixed.c
        gcc -o test$test_name /tmp/test1_fixed.c
    else
        gcc -o test$test_name $test_file
    fi
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Compilation successful${NC}"
        echo "Running with our malloc library..."
        echo "Command: DYLD_LIBRARY_PATH=. DYLD_INSERT_LIBRARIES=\"./libft_malloc.so\" DYLD_FORCE_FLAT_NAMESPACE=1 ./test$test_name"
        echo "Output:"
        timeout 10s /usr/bin/time -l ./test$test_name 2>&1
        echo -e "${GREEN}Test $test_name completed${NC}"
    else
        echo -e "${RED}Compilation failed for test $test_name${NC}"
    fi
    echo
}

# Run each test
run_test "0" "tests/test0.c"
run_test "1" "tests/test1.c" 
run_test "2" "tests/test2.c"
run_test "3" "tests/test3.c"
run_test "4" "tests/test4.c"

echo -e "${BLUE}=== All Tests Completed ===${NC}"

# Cleanup
rm -f test0 test1 test2 test3 test4 /tmp/test1_fixed.c
