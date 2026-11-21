#include <iostream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

enum Genre { fiction, nonfiction, periodical, biography, children };

string genreName(Genre g) {
    if (g == fiction)    return "fiction";
    if (g == nonfiction) return "nonfiction";
    if (g == periodical) return "periodical";
    if (g == biography)  return "biography";
    return "children";
}

// ---------------- DATE ----------------
class Date {
private:
    int day, month, year;
public:
    Date() {   // default: today's date from the computer clock
        time_t now = time(0);
        tm* t = localtime(&now);
        day = t->tm_mday;
        month = t->tm_mon + 1;
        year = t->tm_year + 1900;
    }
    Date(int d, int m, int y) { day = d; month = m; year = y; }
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
};

ostream& operator<<(ostream& out, const Date& d) {
    out << d.getDay() << "/" << d.getMonth() << "/" << d.getYear();
    return out;
}

// ---------------- BOOK ----------------
class Book {
private:
    string isbn, title, author;
    int copyrightDate;
    bool checkedOut;
    Genre genre;
public:
    Book() {   // default constructor: an empty book
        isbn = ""; title = ""; author = "";
        copyrightDate = 0; genre = fiction; checkedOut = false;
    }
    Book(string i, string t, string a, int date, Genre g) {
        isbn = i; title = t; author = a;
        copyrightDate = date; genre = g; checkedOut = false;
    }

    string getIsbn() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    int getCopyrightDate() const { return copyrightDate; }
    bool isCheckedOut() const { return checkedOut; }
    Genre getGenre() const { return genre; }

    void checkOut() { checkedOut = true; }
    void checkIn()  { checkedOut = false; }

    bool operator==(const Book& other) const { return isbn == other.isbn; }
    bool operator!=(const Book& other) const { return isbn != other.isbn; }
};

ostream& operator<<(ostream& out, const Book& b) {
    out << "Title:  " << b.getTitle() << endl;
    out << "Author: " << b.getAuthor() << endl;
    out << "ISBN:   " << b.getIsbn() << endl;
    out << "Genre:  " << genreName(b.getGenre()) << endl;
    return out;
}

// ---------------- PATRON ----------------
class Patron {
private:
    string userName, cardNumber;
    int owedFees;
public:
    Patron() { userName = ""; cardNumber = ""; owedFees = 0; }
    Patron(string name, string card, int fees) {
        userName = name; cardNumber = card; owedFees = fees;
    }
    string getUserName() const { return userName; }
    string getCardNumber() const { return cardNumber; }
    int getOwedFees() const { return owedFees; }
    bool owesFees() const { return owedFees > 0; }
    void setFees(int fees) { owedFees = fees; }
};

// ---------------- TRANSACTION ----------------
class Transaction {
public:
    Book book;
    Patron patron;
    string activity;   // "check out" or "check in"
    Date date;
    Transaction(Book b, Patron p, string act, Date d)
        : book(b), patron(p), activity(act), date(d) {}
};

// ---------------- LIBRARY ----------------
class Library {
private:
    vector<Book> books;
    vector<Patron> patrons;
    vector<Transaction> transactions;

    int findBook(string isbn) const {
        for (int i = 0; i < (int)books.size(); i++)
            if (books[i].getIsbn() == isbn) return i;
        return -1;
    }
    int findPatron(string card) const {
        for (int i = 0; i < (int)patrons.size(); i++)
            if (patrons[i].getCardNumber() == card) return i;
        return -1;
    }

public:
    void addBook(Book b) { books.push_back(b); }
    void addPatron(Patron p) { patrons.push_back(p); }

    void checkOutBook(string isbn, string card) {
        int p = findPatron(card);
        int b = findBook(isbn);
        if (p == -1) { cout << "Error: patron not found." << endl; return; }
        if (b == -1) { cout << "Error: book not found." << endl; return; }
        if (patrons[p].owesFees()) {
            cout << "Error: " << patrons[p].getUserName() << " owes fees of "
                 << patrons[p].getOwedFees() << "." << endl;
            return;
        }
        if (books[b].isCheckedOut()) {
            cout << "Error: the book is already checked out." << endl;
            return;
        }
        books[b].checkOut();
        transactions.push_back(Transaction(books[b], patrons[p], "check out", Date()));
        cout << patrons[p].getUserName() << " checked out \""
             << books[b].getTitle() << "\"." << endl;
    }

    void checkInBook(string isbn, string card) {
        int p = findPatron(card);
        int b = findBook(isbn);
        if (p == -1) { cout << "Error: patron not found." << endl; return; }
        if (b == -1) { cout << "Error: book not found." << endl; return; }
        if (!books[b].isCheckedOut()) {
            cout << "Error: the book is already on the shelf." << endl;
            return;
        }
        books[b].checkIn();
        transactions.push_back(Transaction(books[b], patrons[p], "check in", Date()));
        cout << patrons[p].getUserName() << " returned \""
             << books[b].getTitle() << "\"." << endl;
    }

    void setPatronFees(string card, int fees) {
        int p = findPatron(card);
        if (p == -1) { cout << "Error: patron not found." << endl; return; }
        patrons[p].setFees(fees);
        cout << "Fees updated." << endl;
    }

    vector<string> getPatronsWithFees() const {
        vector<string> names;
        for (int i = 0; i < (int)patrons.size(); i++)
            if (patrons[i].owesFees()) names.push_back(patrons[i].getUserName());
        return names;
    }

    void showBooks() const {
        for (int i = 0; i < (int)books.size(); i++) {
            cout << "\n" << books[i];
            cout << "Status: " << (books[i].isCheckedOut() ? "checked out" : "on the shelf") << endl;
        }
    }

    void showTransactions() const {
        if (transactions.size() == 0) cout << "No transactions yet." << endl;
        for (int i = 0; i < (int)transactions.size(); i++) {
            cout << transactions[i].date << "  "
                 << transactions[i].patron.getUserName() << "  "
                 << transactions[i].activity << "  \""
                 << transactions[i].book.getTitle() << "\"" << endl;
        }
    }
};

// ---------------- MAIN ----------------
int main() {
    Library library;

    // Starting data (add more here if you like)
    library.addBook(Book("111-1", "Things Fall Apart", "Chinua Achebe", 1958, fiction));
    library.addBook(Book("111-2", "The Secret Garden", "Frances Hodgson Burnett", 1911, children));
    library.addBook(Book("111-3", "What Was the Titanic", "Gail B. Stewart", 2000, nonfiction));
    library.addBook(Book("111-4", "The Diary of a Young Girl", "Anne Frank", 1947, biography));
    library.addBook(Book("111-5", "The New Yorker", "Various", 2023, periodical));
    library.addPatron(Patron("Boris", "C-001", 40));
    library.addPatron(Patron("Alice", "C-002", 0));
    library.addPatron(Patron("charlie", "C-003", 0));
    library.addPatron(Patron("Grace", "C-004", 20));

    cout << "Welcome to the Library!" << endl;

    string choice;
    while (true) {
        cout << "\n1. Show books    2. Check out    3. Check in" << endl;
        cout << "4. Set fees      5. Who owes fees    6. Transactions    0. Exit" << endl;
        cout << "Your choice: ";
        if (!(cin >> choice) || choice == "0") break;

        string card, isbn;
        if (choice == "1") {
            library.showBooks();
        } else if (choice == "2") {
            cout << "Card number: "; cin >> card;
            cout << "Book ISBN: ";   cin >> isbn;
            library.checkOutBook(isbn, card);
        } else if (choice == "3") {
            cout << "Card number: "; cin >> card;
            cout << "Book ISBN: ";   cin >> isbn;
            library.checkInBook(isbn, card);
        } else if (choice == "4") {
            int fees;
            cout << "Card number: "; cin >> card;
            cout << "New fees: ";    cin >> fees;
            if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); cout << "Please type a number." << endl; }
            else library.setPatronFees(card, fees);
        } else if (choice == "5") {
            vector<string> names = library.getPatronsWithFees();
            if (names.size() == 0) cout << "Nobody owes fees." << endl;
            for (int i = 0; i < (int)names.size(); i++) cout << names[i] << endl;
        } else if (choice == "6") {
            library.showTransactions();
        } else {
            cout << "Please choose from the menu." << endl;
        }
    }

    cout << "Goodbye!" << endl;
    return 0;
}