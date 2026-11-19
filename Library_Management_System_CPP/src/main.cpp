#include <iostream>
#include <vector>
using namespace std;

class Book
{
private:
    int id;
    string title;
    bool available;

public:
    Book(int bookId, string bookTitle)
    {
        id = bookId;
        title = bookTitle;
        available = true;
    }

    int getId()
    {
        return id;
    }

    string getTitle()
    {
        return title;
    }

    bool isAvailable()
    {
        return available;
    }

    void issueBook()
    {
        if (available)
        {
            available = false;
            cout << "Book Issued Successfully!\n";
        }
        else
        {
            cout << "Book Already Issued!\n";
        }
    }

    void returnBook()
    {
        if (!available)
        {
            available = true;
            cout << "Book Returned Successfully!\n";
        }
        else
        {
            cout << "Book is already in library!\n";
        }
    }

    void display()
    {
        cout << "ID: " << id
             << " | Title: " << title
             << " | Status: "
             << (available ? "Available" : "Issued")
             << endl;
    }
};

int main()
{
    vector<Book> books;
    int choice;

    do
    {
        cout << "\n===== LIBRARY MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Book\n";
        cout << "2. Display Books\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "0. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int id;
            string title;

            cout << "Enter Book ID: ";
            cin >> id;

            cin.ignore();

            cout << "Enter Book Title: ";
            getline(cin, title);

            books.push_back(Book(id, title));

            cout << "Book Added Successfully!\n";
        }

        else if (choice == 2)
        {
            for (int i = 0; i < books.size(); i++)
            {
                books[i].display();
            }
        }

        else if (choice == 3)
        {
            int id;
            cout << "Enter Book ID to Issue: ";
            cin >> id;

            for (int i = 0; i < books.size(); i++)
            {
                if (books[i].getId() == id)
                {
                    books[i].issueBook();
                }
            }
        }

        else if (choice == 4)
        {
            int id;
            cout << "Enter Book ID to Return: ";
            cin >> id;

            for (int i = 0; i < books.size(); i++)
            {
                if (books[i].getId() == id)
                {
                    books[i].returnBook();
                }
            }
        }

    } while (choice != 0);

    return 0;
}
