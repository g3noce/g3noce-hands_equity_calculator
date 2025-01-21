#!/bin/bash

echo "Building main Program..."

# Create directories if they don't exist
mkdir -p obj bin

# Compile each source file
echo "Compiling source files..."
cd src || exit
gcc -c deck.c -o ../obj/deck.o
gcc -c hand_evaluation.c -o ../obj/hand_evaluation.o
gcc -c lookup_tables.c -o ../obj/lookup_tables.o
gcc -c main.c -o ../obj/main.o
cd ..

# Link all object files
echo "Linking..."
gcc obj/*.o -o bin/main

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful! Executable created at bin/main"

# Run the program if --run argument is provided
if [ "$1" = "--run" ]; then
    echo "Running main..."
    cd bin || exit
    ./main
    cd ..
fi

rm -f obj/main.o
