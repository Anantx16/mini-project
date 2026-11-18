# UML Class Diagram

The following diagram describes the main classes and their relationships. The editable PlantUML source is in `uml_class_diagram.puml`.

```text
                 +----------------------+
                 |   <<abstract>>       |
                 |     LibraryUser      |
                 +----------------------+
                 | # id : int           |
                 | # name : string      |
                 +----------------------+
                 | + borrowingLimit()   |
                 | + role()             |
                 | + display()          |
                 | + serialize()        |
                 +----------^-----------+
                            |
              +-------------+-------------+
              |                           |
    +-------------------+       +-------------------+
    |   StudentMember   |       |   FacultyMember   |
    +-------------------+       +-------------------+
    | limit = 3         |       | limit = 5         |
    +-------------------+       +-------------------+

+------------------+       +----------------------+
|       Book       |       |        Library        |
+------------------+       +----------------------+
| id               |       | vector<Book>         |
| title            |<----->| map<int, LibraryUser> |
| author           |       | vector<Loan>          |
| totalCopies      |       +----------------------+
| availableCopies  |       | addBook()             |
+------------------+       | issueBook()           |
                           | returnBook()          |
                           | saveAll()/loadAll()   |
                           +-----------+----------+
                                       |
                                  +----v----+
                                  |  Loan   |
                                  +---------+
                                  | bookId  |
                                  | userId  |
                                  +---------+
```

Relationships:
- `StudentMember` and `FacultyMember` inherit from `LibraryUser`.
- `Library` manages collections of books, users, and active loans.
- Each `Loan` links one book ID with one member ID.
