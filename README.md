# C++ Library Manager

A small C++17 course project that models books, users and borrow records through a console interface.

## Features

- Add and search books by id, title or author
- Register users and validate duplicate ids
- Borrow and return books with inventory checks
- Prevent duplicate borrowing and invalid return operations
- Persist books, users and borrow records as local text files

## Design

- Object-oriented domain model: `Book`, `BorrowRecord` and `Library`
- STL containers: `unordered_map`, `unordered_set` and `vector`
- File persistence with `std::filesystem` and TSV files
- Input and business-boundary validation with clear error messages

## Build and run

```bash
g++ -std=c++17 main.cpp Library.cpp -o library_manager
./library_manager
```

Or use CMake:

```bash
cmake -S . -B build
cmake --build build
./build/library_manager
```

Run a deterministic smoke demo:

```bash
./library_manager --demo
```
