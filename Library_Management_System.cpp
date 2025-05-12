#include <iostream>
#include <string>

using namespace std;

struct Borrower {
    string name;
    string contact;
};

struct Book {
    int id;
    string title;
    string author;
    string category;
    bool isAvailable;
    Borrower borrower;
};

class Library {
private:
    Book books[100];
    int bookCount;

    int findBookIndex(int id) {
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                return i;
            }
        }
        return -1;
    }

public:
    Library() {
        bookCount = 0;
    }

    void addBook() {
        if (bookCount >= 100) {
            cout << "Library is full.\n";
            return;
        }

        Book book;
        cout << "Enter Book ID: ";
        cin >> book.id;
        cin.ignore();

        if (findBookIndex(book.id) != -1) {
            cout << "Book with this ID already exists.\n";
            return;
        }

        cout << "Enter Title: ";
        getline(cin, book.title);

        cout << "Enter Author: ";
        getline(cin, book.author);

        cout << "Enter Category: ";
        getline(cin, book.category);

        book.isAvailable = true;
        books[bookCount++] = book;

        cout << "Book added successfully.\n";
    }

    void viewBooks() {
        if (bookCount == 0) {
            cout << "No books available.\n";
            return;
        }

        for (int i = 0; i < bookCount; i++) {
            cout << "ID: " << books[i].id << ", Title: " << books[i].title
                 << ", Author: " << books[i].author
                 << ", Category: " << books[i].category
                 << ", Status: " << (books[i].isAvailable ? "Available" : "Borrowed") << endl;
        }
    }

    void searchBook() {
        int id;
        cout << "Enter Book ID to search: ";
        cin >> id;

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        Book& book = books[index];
        cout << "Book Details:\n";
        cout << "ID: " << book.id << endl;
        cout << "Title: " << book.title << endl;
        cout << "Author: " << book.author << endl;
        cout << "Category: " << book.category << endl;
        cout << "Status: " << (book.isAvailable ? "Available" : "Borrowed") << endl;

        if (!book.isAvailable) {
            cout << "Borrower Name: " << book.borrower.name << endl;
            cout << "Borrower Contact: " << book.borrower.contact << endl;
        }
    }

    void borrowBook() {
        int id;
        cout << "Enter Book ID to borrow: ";
        cin >> id;
        cin.ignore();

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        if (!books[index].isAvailable) {
            cout << "Book is already borrowed.\n";
            return;
        }

        cout << "Enter Borrower Name: ";
        getline(cin, books[index].borrower.name);

        cout << "Enter Borrower Contact: ";
        getline(cin, books[index].borrower.contact);

        books[index].isAvailable = false;
        cout << "Book borrowed successfully.\n";
    }

    void returnBook() {
        int id;
        cout << "Enter Book ID to return: ";
        cin >> id;

        int index = findBookIndex(id);
        if (index == -1) {
            cout << "Book not found.\n";
            return;
        }

        if (books[index].isAvailable) {
            cout << "Book is not currently borrowed.\n";
            return;
        }

        books[index].isAvailable = true;
        books[index].borrower = Borrower();

        cout << "Book returned successfully.\n";
    }

    void filterByCategory() {
        string category;
        cout << "Enter Category to filter: ";
        cin.ignore();
        getline(cin, category);

        bool found = false;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].category == category) {
                found = true;
                cout << "ID: " << books[i].id << ", Title: " << books[i].title
                     << ", Author: " << books[i].author
                     << ", Status: " << (books[i].isAvailable ? "Available" : "Borrowed") << endl;
            }
        }

        if (!found) {
            cout << "No books found in this category.\n";
        }
    }
};

void displayMenu() {
    cout << "\nLibrary Menu:\n";
    cout << "1. Add New Book\n";
    cout << "2. View All Books\n";
    cout << "3. Search Book by ID\n";
    cout << "4. Borrow Book\n";
    cout << "5. Return Book\n";
    cout << "6. Filter Books by Category\n";
    cout << "7. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    Library library;
    int choice;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                library.addBook();
                break;
            case 2:
                library.viewBooks();
                break;
            case 3:
                library.searchBook();
                break;
            case 4:
                library.borrowBook();
                break;
            case 5:
                library.returnBook();
                break;
            case 6:
                library.filterByCategory();
                break;
            case 7:
                cout << "Exiting Library.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    return 0;
}
