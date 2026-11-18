#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// ----------------------------- Domain classes -----------------------------
class Book {
private:
    int id{};
    string title, author;
    int totalCopies{}, availableCopies{};

public:
    Book() = default;
    Book(int bookId, string bookTitle, string bookAuthor, int copies)
        : id(bookId), title(std::move(bookTitle)), author(std::move(bookAuthor)),
          totalCopies(copies), availableCopies(copies) {
        if (copies < 0) throw invalid_argument("Copies cannot be negative.");
    }

    int getId() const { return id; }
    const string& getTitle() const { return title; }
    const string& getAuthor() const { return author; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }

    void issueCopy() {
        if (availableCopies <= 0) throw runtime_error("No copies are currently available.");
        --availableCopies;
    }
    void returnCopy() {
        if (availableCopies >= totalCopies) throw runtime_error("All copies are already in the library.");
        ++availableCopies;
    }
    void display() const {
        cout << left << setw(8) << id << setw(30) << title.substr(0, 28)
             << setw(24) << author.substr(0, 22) << setw(10) << availableCopies
             << totalCopies << '\n';
    }
    string serialize() const {
        ostringstream out;
        out << id << '|' << title << '|' << author << '|'
            << totalCopies << '|' << availableCopies;
        return out.str();
    }
    static Book deserialize(const string& line) {
        stringstream ss(line);
        string idText, title, author, totalText, availableText;
        if (!getline(ss, idText, '|') || !getline(ss, title, '|') ||
            !getline(ss, author, '|') || !getline(ss, totalText, '|') ||
            !getline(ss, availableText, '|'))
            throw runtime_error("Invalid book record.");
        Book b(stoi(idText), title, author, stoi(totalText));
        int available = stoi(availableText);
        if (available < 0 || available > b.totalCopies) throw runtime_error("Invalid copy count.");
        while (b.availableCopies > available) b.issueCopy();
        return b;
    }
};

class LibraryUser {
protected:
    int id{};
    string name;

public:
    LibraryUser(int userId, string userName) : id(userId), name(std::move(userName)) {}
    virtual ~LibraryUser() = default;
    int getId() const { return id; }
    const string& getName() const { return name; }
    virtual int borrowingLimit() const = 0;
    virtual string role() const = 0;
    virtual void display() const {
        cout << left << setw(8) << id << setw(28) << name << role()
             << " (limit: " << borrowingLimit() << ")\n";
    }
    virtual string serialize() const = 0;
};

class StudentMember : public LibraryUser {
public:
    StudentMember(int id, string name) : LibraryUser(id, std::move(name)) {}
    int borrowingLimit() const override { return 3; }
    string role() const override { return "Student"; }
    string serialize() const override { return to_string(id) + "|S|" + name; }
};

class FacultyMember : public LibraryUser {
public:
    FacultyMember(int id, string name) : LibraryUser(id, std::move(name)) {}
    int borrowingLimit() const override { return 5; }
    string role() const override { return "Faculty"; }
    string serialize() const override { return to_string(id) + "|F|" + name; }
};

struct Loan {
    int bookId{}, userId{};
    string serialize() const { return to_string(bookId) + "|" + to_string(userId); }
};

// ----------------------------- Library controller -----------------------------
class Library {
private:
    vector<Book> books;
    map<int, unique_ptr<LibraryUser>> users;
    vector<Loan> loans;

    const string booksFile = "data/books.txt";
    const string usersFile = "data/users.txt";
    const string loansFile = "data/loans.txt";

    Book* findBook(int bookId) {
        auto it = find_if(books.begin(), books.end(),
                          [bookId](const Book& b) { return b.getId() == bookId; });
        return it == books.end() ? nullptr : &(*it);
    }

    int activeLoansFor(int userId) const {
        return static_cast<int>(count_if(loans.begin(), loans.end(),
            [userId](const Loan& l) { return l.userId == userId; }));
    }

    void ensureDataDirectory() const {
        // The project ships with the data directory. Keep this function as a clear
        // reminder that the application expects to run from the project root.
    }

public:
    Library() { loadAll(); }

    void loadAll() {
        ensureDataDirectory();
        string line;
        ifstream bf(booksFile);
        while (getline(bf, line)) {
            if (line.empty()) continue;
            try { books.push_back(Book::deserialize(line)); }
            catch (const exception& e) { cerr << "Skipping invalid book row: " << e.what() << '\n'; }
        }

        ifstream uf(usersFile);
        while (getline(uf, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idText, type, name;
            if (!getline(ss, idText, '|') || !getline(ss, type, '|') || !getline(ss, name)) continue;
            try {
                int userId = stoi(idText);
                if (type == "S") users[userId] = make_unique<StudentMember>(userId, name);
                else if (type == "F") users[userId] = make_unique<FacultyMember>(userId, name);
            } catch (...) { cerr << "Skipping invalid user row.\n"; }
        }

        ifstream lf(loansFile);
        while (getline(lf, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string bookText, userText;
            if (getline(ss, bookText, '|') && getline(ss, userText, '|')) {
                try { loans.push_back({stoi(bookText), stoi(userText)}); }
                catch (...) { cerr << "Skipping invalid loan row.\n"; }
            }
        }
    }

    void saveAll() const {
        ofstream bf(booksFile);
        if (!bf) throw runtime_error("Cannot write data/books.txt");
        for (const auto& b : books) bf << b.serialize() << '\n';

        ofstream uf(usersFile);
        if (!uf) throw runtime_error("Cannot write data/users.txt");
        for (const auto& pair : users) uf << pair.second->serialize() << '\n';

        ofstream lf(loansFile);
        if (!lf) throw runtime_error("Cannot write data/loans.txt");
        for (const auto& loan : loans) lf << loan.serialize() << '\n';
    }

    void addBook() {
        int id, copies;
        string title, author;
        cout << "Book ID: "; cin >> id;
        if (findBook(id)) throw runtime_error("A book with this ID already exists.");
        cout << "Title: "; getline(cin >> ws, title);
        cout << "Author: "; getline(cin, author);
        cout << "Number of copies: "; cin >> copies;
        if (title.empty() || author.empty() || copies <= 0)
            throw invalid_argument("Title/author must not be empty and copies must be positive.");
        books.emplace_back(id, title, author, copies);
        saveAll();
        cout << "Book added successfully.\n";
    }

    void registerUser() {
        int id, type;
        string name;
        cout << "Member ID: "; cin >> id;
        if (users.count(id)) throw runtime_error("A member with this ID already exists.");
        cout << "Member name: "; getline(cin >> ws, name);
        if (name.empty()) throw invalid_argument("Name cannot be empty.");
        cout << "1. Student (limit 3)  2. Faculty (limit 5)\nChoose type: ";
        cin >> type;
        if (type == 1) users[id] = make_unique<StudentMember>(id, name);
        else if (type == 2) users[id] = make_unique<FacultyMember>(id, name);
        else throw invalid_argument("Invalid member type.");
        saveAll();
        cout << "Member registered successfully.\n";
    }

    void listBooks() const {
        if (books.empty()) { cout << "No books found.\n"; return; }
        cout << left << setw(8) << "ID" << setw(30) << "TITLE" << setw(24)
             << "AUTHOR" << setw(10) << "AVAILABLE" << "TOTAL\n";
        cout << string(82, '-') << '\n';
        for (const auto& b : books) b.display();
    }

    void searchBook() const {
        string keyword;
        cout << "Enter title/author keyword: "; getline(cin >> ws, keyword);
        bool found = false;
        for (const auto& b : books) {
            string title = b.getTitle(), author = b.getAuthor();
            auto contains = [&keyword](string value) {
                transform(value.begin(), value.end(), value.begin(), ::tolower);
                string key = keyword;
                transform(key.begin(), key.end(), key.begin(), ::tolower);
                return value.find(key) != string::npos;
            };
            if (contains(title) || contains(author)) { b.display(); found = true; }
        }
        if (!found) cout << "No matching book found.\n";
    }

    void listUsers() const {
        if (users.empty()) { cout << "No members registered.\n"; return; }
        cout << "ID      MEMBER NAME                 CATEGORY\n" << string(55, '-') << '\n';
        for (const auto& pair : users) pair.second->display();
    }

    void issueBook() {
        int bookId, userId;
        cout << "Book ID: "; cin >> bookId;
        cout << "Member ID: "; cin >> userId;
        Book* book = findBook(bookId);
        auto userIt = users.find(userId);
        if (!book) throw runtime_error("Book ID not found.");
        if (userIt == users.end()) throw runtime_error("Member ID not found.");
        if (activeLoansFor(userId) >= userIt->second->borrowingLimit())
            throw runtime_error("Member has reached the borrowing limit.");
        if (any_of(loans.begin(), loans.end(), [bookId, userId](const Loan& l) {
            return l.bookId == bookId && l.userId == userId;
        })) throw runtime_error("This member has already borrowed this book.");
        book->issueCopy();
        loans.push_back({bookId, userId});
        saveAll();
        cout << "Book issued successfully to " << userIt->second->getName() << ".\n";
    }

    void returnBook() {
        int bookId, userId;
        cout << "Book ID: "; cin >> bookId;
        cout << "Member ID: "; cin >> userId;
        Book* book = findBook(bookId);
        if (!book) throw runtime_error("Book ID not found.");
        auto it = find_if(loans.begin(), loans.end(), [bookId, userId](const Loan& l) {
            return l.bookId == bookId && l.userId == userId;
        });
        if (it == loans.end()) throw runtime_error("No matching active loan found.");
        book->returnCopy();
        loans.erase(it);
        saveAll();
        cout << "Book returned successfully.\n";
    }

    void showLoans() const {
        if (loans.empty()) { cout << "No active loans.\n"; return; }
        cout << "BOOK ID    MEMBER ID\n" << string(25, '-') << '\n';
        for (const auto& l : loans) cout << left << setw(11) << l.bookId << l.userId << '\n';
    }
};

// ---------------------------------- User interface ----------------------------------
void showMenu() {
    cout << "\n========== UNIVERSITY LIBRARY MANAGEMENT SYSTEM ==========\n"
         << "1. Add a new book\n"
         << "2. Register a member\n"
         << "3. Display all books\n"
         << "4. Search books\n"
         << "5. Display members\n"
         << "6. Issue a book\n"
         << "7. Return a book\n"
         << "8. View active loans\n"
         << "0. Save and exit\n"
         << "Select an option: ";
}

int main() {
    try {
        Library library;
        cout << "Welcome to the University Library Management System!\n";
        bool running = true;
        while (running) {
            showMenu();
            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Please enter a valid number.\n";
                continue;
            }
            try {
                switch (choice) {
                    case 1: library.addBook(); break;
                    case 2: library.registerUser(); break;
                    case 3: library.listBooks(); break;
                    case 4: library.searchBook(); break;
                    case 5: library.listUsers(); break;
                    case 6: library.issueBook(); break;
                    case 7: library.returnBook(); break;
                    case 8: library.showLoans(); break;
                    case 0: library.saveAll(); running = false; cout << "Data saved. Goodbye!\n"; break;
                    default: cout << "Invalid choice. Try again.\n";
                }
            } catch (const exception& e) {
                cout << "Operation failed: " << e.what() << '\n';
                cin.clear();
                cin.ignore(10000, '\n');
            }
        }
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
