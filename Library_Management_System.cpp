
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <iomanip>

// ANSI color codes for console output
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RESET "\033[0m"

using namespace std;

// Struct to represent a Borrower
struct Borrower {
    string name; // Name of the borrower
    string contact; // Contact info (e.g., email or phone)
};

// Struct to represent a Book
struct Book {
    int id; // Unique ID
    string title; // Book title
    string author; // Author
    string category; // Category (e.g., Fiction, Science)
    bool isAvailable; // Availability status
    Borrower borrower; // Current borrower, if any
    time_t borrowDate; // Date borrowed
};

// Class to manage the Library
class Library {
private:
    vector<Book> books; // Collection of books
    const string dataFile = "library_books.dat"; // File for data persistence
    const int borrowLimitDays = 14; // Max borrowing period
    const double finePerDay = 1.0; // Fine per overdue day
    const char encryptionKey = 'X'; // Simple XOR key for file encryption

    // Helper to encrypt/decrypt string (simple XOR)
    string cipherString(const string& text) {
        string result = text;
        for (char& c : result) {
            c ^= encryptionKey;
        }
        return result;
    }

    // Load books from file
    void loadBooks() {
        ifstream file(dataFile, ios::binary);
        if (!file.is_open()) return;

        books.clear();
        while (file) {
            Book book;
            string temp;
            getline(file, temp); // Read id
            if (temp.empty()) break;
            book.id = stoi(cipherString(temp));
            getline(file, temp); // Read title
            book.title = cipherString(temp);
            getline(file, temp); // Read author
            book.author = cipherString(temp);
            getline(file, temp); // Read category
            book.category = cipherString(temp);
            getline(file, temp); // Read availability
            book.isAvailable = (temp == "1");
            if (!book.isAvailable) {
                getline(file, temp); // Read borrower name
                book.borrower.name = cipherString(temp);
                getline(file, temp); // Read borrower contact
                book.borrower.contact = cipherString(temp);
                getline(file, temp); // Read borrow date
                book.borrowDate = stol(cipherString(temp));
            }
            books.push_back(book);
        }
        file.close();
    }

    // Save books to file
    void saveBooks() {
        ofstream file(dataFile, ios::binary);
        for (const Book& book : books) {
            file << cipherString(to_string(book.id)) << '\n';
            file << cipherString(book.title) << '\n';
            file << cipherString(book.author) << '\n';
            file << cipherString(book.category) << '\n';
            file << (book.isAvailable ? "1" : "0") << '\n';
            if (!book.isAvailable) {
                file << cipherString(book.borrower.name) << '\n';
                file << cipherString(book.borrower.contact) << '\n';
                file << cipherString(to_string(book.borrowDate)) << '\n';
            }
        }
        file.close();
    }

    // Check if id is unique
    bool isIdUnique(int id) {
        return none_of(books.begin(), books.end(), 
            [id](const Book& b) { return b.id == id; });
    }

    // Calculate fine for overdue book
    double calculateFine(time_t borrowDate) {
        time_t now = time(nullptr);
        double daysPassed = difftime(now, borrowDate) / (60 * 60 * 24);
        if (daysPassed > borrowLimitDays) {
            return (daysPassed - borrowLimitDays) * finePerDay;
        }
        return 0.0;
    }

public:
    Library() {
        loadBooks(); // Load data on startup
    }

    ~Library() {
        saveBooks(); // Save data on exit
    }

    // Add a new book
    void addBook() {
        Book book;
        cout << COLOR_GREEN << "Adding new book...\n" << COLOR_RESET;
        cout << "Enter ID (unique): ";
        while (!(cin >> book.id) || !isIdUnique(book.id)) {
            cout << COLOR_RED << "Invalid or duplicate ID. Try again: " << COLOR_RESET;
            cin.clear();
            cin.ignore(10000, '\n');
        }
        cin.ignore();
        cout << "Enter Title: ";
        getline(cin, book.title);
        if (book.title.empty()) book.title = "Unknown";
        cout << "Enter Author: ";
        getline(cin, book.author);
        if (book.author.empty()) book.author = "Unknown";
        cout << "Enter Category (e.g., Fiction, Science): ";
        getline(cin, book.category);
        if (book.category.empty()) book.category = "General";
        book.isAvailable = true;

        books.push_back(book);
        saveBooks();
        cout << COLOR_GREEN << "Book added successfully!\n" << COLOR_RESET;
    }

    // View all books
    void viewBooks() {
        if (books.empty()) {
            cout << COLOR_RED << "No books in the library.\n" << COLOR_RESET;
            return;
        }
        cout << COLOR_YELLOW << "\nLibrary Contents:\n" << COLOR_RESET;
        cout << left << setw(10) << "ID" << setw(30) << "Title" 
             << setw(20) << "Author" << setw(15) << "Category" 
             << setw(15) << "Status" << "Fine\n";
        cout << string(90, '-') << '\n';
        for (const Book& book : books) {
            cout << left << setw(10) << book.id << setw(30) << book.title 
                 << setw(20) << book.author << setw(15) << book.category 
                 << setw(15) << (book.isAvailable ? "Available" : "Borrowed");
            if (!book.isAvailable) {
                cout << "$" << fixed << setprecision(2) << calculateFine(book.borrowDate);
            }
            cout << '\n';
        }
    }

    // Search book by ID
    void searchBook() {
        int id;
        cout << "Enter ID to search: ";
        if (!(cin >> id)) {
            cout << COLOR_RED << "Invalid ID.\n" << COLOR_RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }
        auto it = find_if(books.begin(), books.end(), 
            [id](const Book& b) { return b.id == id; });
        if (it == books.end()) {
            cout << COLOR_RED << "Book not found.\n" << COLOR_RESET;
            return;
        }
        cout << COLOR_YELLOW << "\nBook Found:\n" << COLOR_RESET;
        cout << "ID: " << it->id << '\n';
        cout << "Title: " << it->title << '\n';
        cout << "Author: " << it->author << '\n';
        cout << "Category: " << it->category << '\n';
        cout << "Status: " << (it->isAvailable ? "Available" : "Borrowed") << '\n';
        if (!it->isAvailable) {
            cout << "Borrower: " << it->borrower.name << " (" << it->borrower.contact << ")\n";
            cout << "Fine: $" << fixed << setprecision(2) << calculateFine(it->borrowDate) << '\n';
        }
    }

    // Borrow a book
    void borrowBook() {
        int id;
        cout << "Enter ID to borrow: ";
        if (!(cin >> id)) {
            cout << COLOR_RED << "Invalid ID.\n" << COLOR_RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }
        auto it = find_if(books.begin(), books.end(), 
            [id](const Book& b) { return b.id == id; });
        if (it == books.end()) {
            cout << COLOR_RED << "Book not found.\n" << COLOR_RESET;
            return;
        }
        if (!it->isAvailable) {
            cout << COLOR_RED << "Book is already borrowed.\n" << COLOR_RESET;
            return;
        }
        cin.ignore();
        cout << "Enter Borrower Name: ";
        getline(cin, it->borrower.name);
        cout << "Enter Borrower Contact: ";
        getline(cin, it->borrower.contact);
        it->isAvailable = false;
        it->borrowDate = time(nullptr);
        saveBooks();
        cout << COLOR_GREEN << "Book borrowed successfully!\n" << COLOR_RESET;
    }

    // Return a book
    void returnBook() {
        int id;
        cout << "Enter ID to return: ";
        if (!(cin >> id)) {
            cout << COLOR_RED << "Invalid ID.\n" << COLOR_RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }
        auto it = find_if(books.begin(), books.end(), 
            [id](const Book& b) { return b.id == id; });
        if (it == books.end()) {
            cout << COLOR_RED << "Book not found.\n" << COLOR_RESET;
            return;
        }
        if (it->isAvailable) {
            cout << COLOR_RED << "Book is not borrowed.\n" << COLOR_RESET;
            return;
        }
        double fine = calculateFine(it->borrowDate);
        it->isAvailable = true;
        it->borrower = Borrower();
        it->borrowDate = 0;
        saveBooks();
        cout << COLOR_GREEN << "Book returned successfully!\n" << COLOR_RESET;
        if (fine > 0) {
            cout << COLOR_YELLOW << "Fine due: $" << fixed << setprecision(2) << fine << COLOR_RESET << '\n';
        }
    }

    // Filter books by category
    void filterByCategory() {
        string category;
        cout << "Enter Category to filter (e.g., Fiction, Science): ";
        cin.ignore();
        getline(cin, category);
        cout << COLOR_YELLOW << "\nBooks in Category '" << category << "':\n" << COLOR_RESET;
        bool found = false;
        for (const Book& book : books) {
            if (book.category == category) {
                found = true;
                cout << "ID: " << book.id << ", Title: " << book.title 
                     << ", Author: " << book.author << ", Status: " 
                     << (book.isAvailable ? "Available" : "Borrowed") << '\n';
            }
        }
        if (!found) {
            cout << COLOR_RED << "No books found in this category.\n" << COLOR_RESET;
        }
    }
};

// Menu-driven interface
void displayMenu() {
    cout << COLOR_YELLOW << "\nLibrary Menu:\n" << COLOR_RESET;
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
    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cout << COLOR_RED << "Invalid input. Please enter a number.\n" << COLOR_RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        switch (choice) {
            case 1: library.addBook(); break;
            case 2: library.viewBooks(); break;
            case 3: library.searchBook(); break;
            case 4: library.borrowBook(); break;
            case 5: library.returnBook(); break;
            case 6: library.filterByCategory(); break;
            case 7: cout << COLOR_GREEN << "Exiting Library.\n" << COLOR_RESET; return 0;
            default: cout << COLOR_RED << "Invalid choice. Try again.\n" << COLOR_RESET;
        }
    }
}