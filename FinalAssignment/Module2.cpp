#include "Globals.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;

bool validDate(string date);
string formatDate(string date);

int findMemberIndexById(const string& memberId);

// STUDENT ID VALIDATION

bool validStudentID(string studentID) {

    if (studentID.length() != 7)
        return false;

    for (char c : studentID) {
        if (!isdigit(c))
            return false;
    }

    return true;
}


bool validStaffID(string staffID)
{
    if (staffID.length() != 4)
        return false;

    if (staffID[0] != 'S')
        return false;

    for (int i = 1; i < 4; i++) {
        if (!isdigit(staffID[i]))
            return false;
    }

    return true;
}


string getStudentID() {
    string id;

    while (true) {
        cout << "Student ID(2504668) : ";
        cin >> id;

        if (validStudentID(id)) {
            return id;
        }
        else {
            cout << "Invalid Student ID. Enter 7 digits.\n";
        }
    }
}


string getStaffID() {
    string id;

    while (true) {
        cout << "Staff ID : ";
        cin >> id;

        if (validStaffID(id)) {
            return id;
        }
        else {
            cout << "Invalid Staff ID. Format: S001\n";
        }
    }
}


// SAVE BOOK FILE

void saveFile() {

    ofstream file(bookFile);

    for (Book b : books) {

        file << b.id << endl;
        file << b.title << endl;
        file << b.author << endl;
        file << b.isbn << endl;
        file << b.category << endl;
        file << b.totalCopies << endl;
        file << b.availableCopies << endl;
    }

    file.close();
}



// LOAD BOOK FILE


void loadFile() {

    books.clear();

    ifstream file(bookFile);

    if (!file)
        return;

    Book b;

    while (file >> b.id) {

        file.ignore(1000, '\n');

        getline(file, b.title);
        getline(file, b.author);
        getline(file, b.isbn);
        getline(file, b.category);

        file >> b.totalCopies;
        file >> b.availableCopies;
        file.ignore();

        books.push_back(b);
    }

    file.close();
}


// SAVE BORROW FILE


void saveBorrowFile() {

    ofstream file(borrowFile);

    for (BorrowRecord r : borrowRecords) {

        file << r.memberID << endl;
        file << r.bookID << endl;
        file << r.borrowDate << endl;
        file << r.dueDate << endl;
        file << r.returnDate << endl;
        file << r.returned << endl;
        file << r.renewed << endl;
    }

    file.close();
}



// LOAD BORROW FILE


void loadBorrowFile() {
    borrowRecords.clear();

    ifstream file(borrowFile);

    if (!file)
        return;

    BorrowRecord r;

    while (file >> r.memberID) {

        file >> r.bookID;
        file.ignore(1000, '\n');

        getline(file, r.borrowDate);
        getline(file, r.dueDate);
        getline(file, r.returnDate);

        if (validDate(r.borrowDate)) {
            r.borrowDate = formatDate(r.borrowDate);
        }

        if (validDate(r.dueDate)) {
            r.dueDate = formatDate(r.dueDate);
        }

        if (!r.returnDate.empty() && validDate(r.returnDate)) {
            r.returnDate = formatDate(r.returnDate);
        }

        file >> r.returned;
        file >> r.renewed;
        file.ignore();

        borrowRecords.push_back(r);
    }

    file.close();
}



// GENERATE BOOK ID


int generateBookID() {

    if (books.empty())
        return 1001;

    return books.back().id + 1;
}



// DUPLICATE ISBN


bool duplicateISBN(string isbn) {

    for (Book b : books) {

        if (b.isbn == isbn)
            return true;
    }

    return false;
}



// ADD BOOK


void addBook() {

    Book b;

    b.id = generateBookID();

    cin.ignore();

    cout << "Title : ";
    getline(cin, b.title);

    while (b.title.empty()) {

        cout << "Title cannot be empty.\n";
        getline(cin, b.title);
    }

    cout << "Author : ";
    getline(cin, b.author);

    while (b.author.empty()) {

        cout << "Author cannot be empty.\n";
        getline(cin, b.author);
    }

    cout << "ISBN : ";
    getline(cin, b.isbn);

    while (b.isbn.empty()) {

        cout << "ISBN cannot be empty.\n";
        getline(cin, b.isbn);
    }

    if (duplicateISBN(b.isbn)) {

        cout << "Duplicate ISBN.\n";
        return;
    }

    cout << "Category : ";
    getline(cin, b.category);

    while (b.category.empty()) {

        cout << "Category cannot be empty.\n";
        getline(cin, b.category);
    }



    // TOTAL COPIES


    while (true) {

        cout << "Total Copies : ";

        cin >> b.totalCopies;

        if (cin.fail()) {

            cout << "Invalid input. Please enter an integer.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            continue;
        }

        if (b.totalCopies <= 0) {

            cout << "Invalid input. Enter a positive number.\n";
            continue;
        }

        break;
    }



    // AVAILABLE COPIES

    while (true) {

        cout << "Available Copies : ";

        cin >> b.availableCopies;

        if (cin.fail()) {

            cout << "Invalid input. Please enter an integer.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            continue;
        }

        if (b.availableCopies < 0 || b.availableCopies > b.totalCopies) {

            cout << "Invalid input. Available copies must be between 0 and Total Copies.\n";

            continue;
        }

        break;
    }


    books.push_back(b);

    saveFile();

    cout << "\nBook Added Successfully.\n";
}


// LOWERCASE

string toLower(string text) {

    transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return tolower(c);
        });

    return text;
}


// SEARCH BOOK

void searchBook() {
    cin.clear();
    cin.ignore(1000, '\n');

    int choice = -1;
    string keyword;
    bool found = false;

    cout << "\n====== Search Book ======\n";
    cout << "1. Search by Title\n";
    cout << "2. Search by Author\n";
    cout << "3. Search by Category\n";
    cout << "0. Back\n";

    while (true) {
        cout << "Enter choice : ";

        if (!(cin >> choice)) {
            if (cin.eof()) {
                return;
            }

            cout << "Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            continue;
        }

        if (choice == 0)
            return;

        if (choice >= 1 && choice <= 3)
            break;

        cout << "Please enter 0, 1, 2 or 3.\n";
    }

    cin.ignore(1000, '\n');

    cout << "Enter keyword : ";
    getline(cin, keyword);

    if (keyword.empty()) {
        cout << "Keyword cannot be empty.\n";
        return;
    }

    string search = toLower(keyword);

    cout << endl;

    for (Book b : books) {

        bool match = false;

        if (choice == 1) {

            if (toLower(b.title).find(search) != string::npos)
                match = true;
        }
        else if (choice == 2) {
            if (toLower(b.author).find(search) != string::npos)
                match = true;
        }
        else if (choice == 3) {
            if (toLower(b.category).find(search) != string::npos)
                match = true;
        }

        if (match) {

            found = true;

            cout << "Book ID          : " << b.id << endl;
            cout << "Title            : " << b.title << endl;
            cout << "Author           : " << b.author << endl;
            cout << "ISBN             : " << b.isbn << endl;
            cout << "Category         : " << b.category << endl;
            cout << "Total Copies     : " << b.totalCopies << endl;
            cout << "Available Copies : " << b.availableCopies << endl;
            cout << "----------------------------------------" << endl;
        }
    }

    if (!found) {

        cout << "Book not found.\n";
    }
}



// VIEW BOOKS

void viewBooks() {
    if (books.empty()) {

        cout << "\nNo books available.\n";
        return;
    }

    cout << left
        << setw(8) << "ID"
        << setw(30) << "Title"
        << setw(20) << "Author"
        << setw(18) << "ISBN"
        << setw(18) << "Category"
        << setw(10) << "Total"
        << setw(10) << "Available"
        << endl;

    cout << string(114, '-') << endl;

    for (Book b : books) {

        cout << left
            << setw(8) << b.id
            << setw(30) << b.title
            << setw(20) << b.author
            << setw(18) << b.isbn
            << setw(18) << b.category
            << setw(10) << b.totalCopies
            << setw(10) << b.availableCopies
            << endl;
    }
}



// VALID DATE

bool validDate(string date) {

    int day, month, year;
    char slash1, slash2;
    char extra;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    if (ss.fail())
        return false;

    if (slash1 != '/' || slash2 != '/')
        return false;

    if (ss >> extra)
        return false;

    if (month < 1 || month > 12)
        return false;

    if (year < 2000 || year > 2100)
        return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (year % 4 == 0) {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    return true;
}

string formatDate(string date) {

    int day, month, year;
    char slash1, slash2;

    stringstream ss(date);
    ss >> day >> slash1 >> month >> slash2 >> year;

    stringstream output;
    output << setfill('0')
        << setw(2) << day << "/"
        << setw(2) << month << "/"
        << setw(4) << year;

    return output.str();
}



// CONVERT DATE TO NUMBER

int dateToNumber(string date) {

    int day, month, year;
    char slash1, slash2;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    return year * 365 + month * 30 + day;
}



// CHECK PAST DATE

bool isPastDate(string date) {

    int day, month, year;
    char slash1, slash2;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    // Today's date (shared constant from Globals.h)
    int todayDay = TODAY_DAY;
    int todayMonth = TODAY_MONTH;
    int todayYear = TODAY_YEAR;

    int inputDate =
        year * 365
        + month * 30
        + day;

    int todayDate =
        todayYear * 365
        + todayMonth * 30
        + todayDay;

    if (inputDate < todayDate)
        return true;

    return false;
}


// BORROW BOOK

void borrowBook() {

    int bookID;

    cout << "Book ID : ";

    while (!(cin >> bookID)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    for (Book& b : books) {

        if (b.id == bookID) {

            if (b.availableCopies <= 0) {

                cout << "No copies available.\n";
                return;
            }

            BorrowRecord record;

            record.memberID = getStudentID();

            if (findMemberIndexById(record.memberID) == -1) {

                cout << "\nMember ID not found. Please register the member in\n";
                cout << "Member Management before borrowing a book.\n";
                return;
            }

            cin.ignore();

            // ==========================
            // BORROW DATE
            // ==========================

            cout << "Borrow Date (DD/MM/YYYY) : ";
            getline(cin, record.borrowDate);

            while (!validDate(record.borrowDate)) {

                cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                getline(cin, record.borrowDate);
            }

            record.borrowDate = formatDate(record.borrowDate);

            if (isPastDate(record.borrowDate)) {

                cout << "Borrow date cannot be before today.\n";
                return;
            }


            // DUE DATE

            cout << "Due Date (DD/MM/YYYY) : ";
            getline(cin, record.dueDate);

            while (!validDate(record.dueDate)) {

                cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                getline(cin, record.dueDate);
            }

            record.dueDate = formatDate(record.dueDate);

            while (dateToNumber(record.dueDate) < dateToNumber(record.borrowDate)) {

                cout << "Due date cannot be before borrow date.\n";

                cout << "Due Date (DD/MM/YYYY) : ";
                getline(cin, record.dueDate);

                while (!validDate(record.dueDate)) {

                    cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                    getline(cin, record.dueDate);
                }

                record.dueDate = formatDate(record.dueDate);
            }


            record.bookID = bookID;
            record.returnDate = "";
            record.returned = false;
            record.renewed = false;

            borrowRecords.push_back(record);

            b.availableCopies--;

            saveFile();
            saveBorrowFile();

            cout << "\nBook Borrowed Successfully.\n";

            return;
        }
    }

    cout << "Invalid Book ID.\n";
}


// CALCULATE LOAN DAYS

int calculateLoanDays(string borrowDate, string returnDate) {

    int borrowDay = 0;
    int borrowMonth = 0;
    int borrowYear = 0;

    int returnDay = 0;
    int returnMonth = 0;
    int returnYear = 0;

    char slash;

    stringstream ss1(borrowDate);

    ss1 >> borrowDay
        >> slash
        >> borrowMonth
        >> slash
        >> borrowYear;

    stringstream ss2(returnDate);

    ss2 >> returnDay
        >> slash
        >> returnMonth
        >> slash
        >> returnYear;

    int borrowTotal =
        borrowYear * 365
        + borrowMonth * 30
        + borrowDay;

    int returnTotal =
        returnYear * 365
        + returnMonth * 30
        + returnDay;

    return returnTotal - borrowTotal;
}


// RETURN BOOK

void returnBook() {

    int bookID;

    cout << "Book ID : ";

    while (!(cin >> bookID)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    for (Book& b : books) {

        if (b.id == bookID) {

            bool foundRecord = false;

            for (BorrowRecord& r : borrowRecords) {

                if (r.bookID == bookID &&
                    r.returned == false) {

                    foundRecord = true;

                    cin.ignore();

                    // RETURN DATE

                    cout << "Return Date (DD/MM/YYYY) : ";
                    getline(cin, r.returnDate);

                    while (!validDate(r.returnDate)) {

                        cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                        getline(cin, r.returnDate);
                    }

                    r.returnDate = formatDate(r.returnDate);

                    while (dateToNumber(r.returnDate) < dateToNumber(r.borrowDate)) {

                        cout << "Return date cannot be before borrow date.\n";

                        cout << "Return Date (DD/MM/YYYY) : ";
                        getline(cin, r.returnDate);

                        while (!validDate(r.returnDate)) {

                            cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                            getline(cin, r.returnDate);
                        }

                        r.returnDate = formatDate(r.returnDate);
                    }


                    int duration = calculateLoanDays(
                        r.borrowDate,
                        r.returnDate
                    );

                    r.returned = true;

                    b.availableCopies++;

                    saveFile();
                    saveBorrowFile();

                    cout << "\nBook Returned Successfully.\n";

                    cout << "Loan Duration : " << duration << " day(s)\n";

                    return;
                }
            }

            if (!foundRecord)
            {
                cout << "No active borrowing record found.\n";
            }

            return;
        }
    }

    cout << "Invalid Book ID.\n";
}


// UPDATE BOOK

void updateBook() {

    int id;

    cout << "Book ID : ";

    // BOOK ID VALIDATION

    while (!(cin >> id)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    Book* target = nullptr;

    for (Book& b : books) {

        if (b.id == id) {
            target = &b;
            break;
        }
    }

    if (target == nullptr) {

        cout << "Book not found.\n";
        return;
    }

    Book& b = *target;

    cin.ignore(1000, '\n');

    int choice;

    do {

        cout << "\n===== Update Book (ID: " << b.id << ") =====\n";
        cout << "1. Title    : " << b.title << "\n";
        cout << "2. Author   : " << b.author << "\n";
        cout << "3. ISBN     : " << b.isbn << "\n";
        cout << "4. Category : " << b.category << "\n";
        cout << "5. Total Copies     : " << b.totalCopies << "\n";
        cout << "6. Available Copies : " << b.availableCopies << "\n";
        cout << "0. Done / Back\n";
        cout << "==========================================\n";
        cout << "Select field to update (0-6): ";

        while (!(cin >> choice)) {

            cout << "Invalid input. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            cout << "Select field to update (0-6): ";
        }

        cin.ignore(1000, '\n');

        switch (choice) {

        case 1: {

            cout << "New Title : ";
            getline(cin, b.title);

            saveFile();
            cout << "Title updated successfully.\n";
            break;
        }

        case 2: {

            cout << "New Author : ";
            getline(cin, b.author);

            saveFile();
            cout << "Author updated successfully.\n";
            break;
        }

        case 3: {

            string newISBN;

            cout << "New ISBN : ";
            getline(cin, newISBN);

            bool duplicate = false;

            for (Book other : books) {

                if (other.isbn == newISBN && other.id != b.id) {

                    duplicate = true;
                    break;
                }
            }

            if (duplicate) {

                cout << "ISBN already exists. ISBN not updated.\n";
            }
            else {

                b.isbn = newISBN;

                saveFile();
                cout << "ISBN updated successfully.\n";
            }

            break;
        }

        case 4: {

            cout << "New Category : ";
            getline(cin, b.category);

            saveFile();
            cout << "Category updated successfully.\n";
            break;
        }

        case 5: {

            int newTotal;

            while (true) {

                cout << "New Total Copies : ";

                if (!(cin >> newTotal)) {

                    cout << "Invalid input. Please enter an integer.\n";

                    cin.clear();
                    cin.ignore(1000, '\n');

                    continue;
                }

                if (newTotal <= 0) {

                    cout << "Invalid total copies. Enter a positive number.\n";
                    continue;
                }

                if (newTotal < b.availableCopies) {

                    cout << "Total copies cannot be less than current available copies (" << b.availableCopies << ").\n";
                    continue;
                }

                break;
            }

            cin.ignore(1000, '\n');

            b.totalCopies = newTotal;

            saveFile();
            cout << "Total copies updated successfully.\n";
            break;
        }

        case 6: {

            int newAvailable;

            while (true) {

                cout << "New Available Copies : ";

                if (!(cin >> newAvailable)) {

                    cout << "Invalid input. Please enter an integer.\n";

                    cin.clear();
                    cin.ignore(1000, '\n');

                    continue;
                }

                if (newAvailable < 0 || newAvailable > b.totalCopies) {

                    cout << "Available copies must be between 0 and total copies.\n";
                    continue;
                }

                break;
            }

            cin.ignore(1000, '\n');

            b.availableCopies = newAvailable;

            saveFile();
            cout << "Available copies updated successfully.\n";
            break;
        }

        case 0:
            cout << "Returning to Book Catalog & Borrowing menu.\n";
            break;

        default:
            cout << "Invalid choice. Please select between 0 and 6.\n";
        }

    } while (choice != 0);
}


// REMOVE BOOK

void removeBook() {

    int id;

    cout << "Book ID : ";

    while (!(cin >> id)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }


    for (Book b : books) {

        if (b.id == id) {

            for (BorrowRecord r : borrowRecords) {

                if (r.bookID == id && r.returned == false) {

                    cout << "Cannot remove this book.\n";
                    cout << "The book is currently borrowed.\n";
                    return;
                }
            }

            for (int i = 0; i < books.size(); i++) {

                if (books[i].id == id) {

                    books.erase(books.begin() + i);

                    saveFile();

                    cout << "Book Removed Successfully.\n";

                    return;
                }
            }
        }
    }

    cout << "Book not found.\n";
}


// MODULE 2 MENU

void module2Menu() {

    loadFile();
    loadBorrowFile();

    int choice = -1;

    do {
        cout << "\n==============================\n";
        cout << " BOOK CATALOG & BORROWING\n";
        cout << "==============================\n";
        cout << "1. Add Book\n";
        cout << "2. View Books\n";
        cout << "3. Search Book\n";
        cout << "4. Borrow Book\n";
        cout << "5. Return Book\n";
        cout << "6. Update Book\n";
        cout << "7. Remove Book\n";
        cout << "0. Back\n";
        cout << "==============================\n";
        cout << "Choice : ";

        // ==============================
        // MENU INPUT VALIDATION
        // ==============================

        if (!(cin >> choice)) {
            if (cin.eof()) {
                return;
            }

            cout << "Invalid choice. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;

            continue;
        }


        switch (choice) {

        case 1:
            addBook();
            break;

        case 2:
            viewBooks();
            break;

        case 3:
            searchBook();
            break;

        case 4:
            borrowBook();
            break;

        case 5:
            returnBook();
            break;

        case 6:
            updateBook();
            break;

        case 7:
            removeBook();
            break;

        case 0:
            break;

        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}