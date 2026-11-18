# University Library Management System (C++)

A console-based C++17 mini project for Experiment 13. It demonstrates object-oriented programming, STL, file handling, exception handling, and a Git-ready project structure.

## Features
- Add books and maintain total/available copy counts.
- Register Student and Faculty members with different borrowing limits.
- Search books by title or author (case-insensitive).
- Issue and return books with validation.
- View members and active loans.
- Persist books, members, and loans in text files between runs.
- Uses inheritance, runtime polymorphism, encapsulation, `vector`, `map`, algorithms, and smart pointers.

## Requirements
- C++17 compiler (GCC 9+, Clang 10+, or a recent MSVC).
- Git (for version control).

## Compile and run

Run these commands from the project root:

### Linux / macOS / MinGW
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o library_system
./library_system
```

### Windows PowerShell (MinGW)
```powershell
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o library_system.exe
.\library_system.exe
```

The program reads and writes `data/books.txt`, `data/users.txt`, and `data/loans.txt`. Run it from the project root so these relative paths resolve correctly.

## First run
The data files are intentionally empty. Add a book and register a member from the menu before issuing a book.

## Suggested test sequence
1. Add book ID `101`, title `C++ Programming`, author `E. Balagurusamy`, copies `3`.
2. Register member ID `501` as Student.
3. Issue book `101` to member `501`.
4. Display books and active loans; available copies should now be `2`.
5. Return book `101` for member `501`; available copies should return to `3`.
6. Exit and reopen the application to verify persistence.

## Project structure
```text
Library_Management_System_CPP/
├── src/main.cpp
├── data/books.txt
├── data/users.txt
├── data/loans.txt
├── docs/uml_class_diagram.puml
├── docs/uml_class_diagram.md
├── .gitignore
└── README.md
```

## OOP concepts used
- **Encapsulation:** Book and LibraryUser data is private/protected and accessed through methods.
- **Inheritance:** StudentMember and FacultyMember inherit from LibraryUser.
- **Abstraction:** LibraryUser defines the common interface and pure virtual methods.
- **Runtime polymorphism:** `display()`, `role()`, `borrowingLimit()`, and `serialize()` are overridden and invoked through base-class pointers.
- **Composition/association:** Library manages books, members, and active loans.

## Important note
This is an academic demonstration, not a production library system. It does not include authentication, fine calculation, due dates, concurrent access, or database-backed transactions. Keep the data files in the project directory.
