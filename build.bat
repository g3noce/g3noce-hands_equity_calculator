@echo off
echo Building main Program...

REM Create directories if they don't exist
if not exist obj mkdir obj
if not exist bin mkdir bin

REM Compile each source file
echo Compiling source files...
cd src
gcc -c deck.c -o ../obj/deck.o
gcc -c hand_evaluation.c -o ../obj/hand_evaluation.o
gcc -c lookup_tables.c -o ../obj/lookup_tables.o
gcc -c main.c -o ../obj/main.o
cd ..

REM Link all object files
echo Linking...
gcc obj/*.o -o bin/main.exe

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Build successful! Executable created at bin/main.exe

REM Run the program if --run argument is provided
if "%1"=="--run" (
    echo Running main.exe...
    cd bin
    main.exe
    cd ..
)

del obj\main.o

pause