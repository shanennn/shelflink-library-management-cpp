#include "Globals.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

void loadFile();
void loadBorrowFile();
void saveBorrowFile();


// MEMBER ID VALIDATION

static bool validStudentID3(string id) {

    if (id.length() != 7)
        return false;

    for (char c : id) {
        if (!isdigit(c))
            return false;
    }

    return true;
}


static bool validStaffID3(string id) {

    if (id.length() != 4)
        return false;

    if (id[0] != 'S')
        return false;

    for (int i = 1; i < 4; i++) {
        if (!isdigit(id[i]))
            return false;
    }

    return true;
}


static string getMemberID3() {
    string id;

    while (true) {
        cout << "Member ID (Student 7 digits / Staff S001) : ";
        cin >> id;

        if (validStudentID3(id) || validStaffID3(id)) {
            return id;
        }
        else {
            cout << "Invalid Member ID.\n";
        }
    }
}


// VALID DATE

static bool validDate3(string date) {

    int day, month, year;
    char slash1, slash2;
    char extra;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    if (ss.fail())
        return false;

    if (slash1 != '/' || slash2 != '/')
        return false;

    if (month < 1 || month > 12)
        return false;

    if (year < 2000 || year > 2100)
        return false;

    if (ss >> extra)
        return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (year % 4 == 0) {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    return true;
}

static string formatDate3(string date) {

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


static int dateToNumber3(string date) {

    int day, month, year;
    char slash1, slash2;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    static const int cumulativeDays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

    int totalDays = year * 365 + (year / 4) - (year / 100) + (year / 400);
    totalDays += cumulativeDays[month - 1];

    if (month > 2 && isLeap) {
        totalDays += 1;
    }

    totalDays += day;

    return totalDays;
}


// TODAY'S DATE (shared constant from Globals.h)

static int todayDateNumber3() {

    static const int cumulativeDays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    bool isLeap = (TODAY_YEAR % 4 == 0 && TODAY_YEAR % 100 != 0) || (TODAY_YEAR % 400 == 0);

    int totalDays = TODAY_YEAR * 365 + (TODAY_YEAR / 4) - (TODAY_YEAR / 100) + (TODAY_YEAR / 400);
    totalDays += cumulativeDays[TODAY_MONTH - 1];

    if (TODAY_MONTH > 2 && isLeap) {
        totalDays += 1;
    }

    totalDays += TODAY_DAY;

    return totalDays;
}


// FIND BOOK INDEX BY ID

static int findBookIndex3(int bookID) {

    for (int i = 0; i < static_cast<int>(books.size()); i++) {

        if (books[i].id == bookID) {
            return i;
        }
    }

    return -1;
}


// FIND OR CREATE RESERVATION QUEUE FOR A BOOK

static int findQueueIndex3(int bookID) {

    for (int i = 0; i < static_cast<int>(bookQueues.size()); i++) {

        if (bookQueues[i].bookID == bookID) {
            return i;
        }
    }

    return -1;
}


static int getOrCreateQueueIndex3(int bookID) {

    int index = findQueueIndex3(bookID);

    if (index != -1)
        return index;

    BookQueue newQueue;

    newQueue.bookID = bookID;
    newQueue.waitCount = 0;

    bookQueues.push_back(newQueue);

    return static_cast<int>(bookQueues.size()) - 1;
}


// RESERVE A BOOK

void reserveBook() {

    int bookID;

    cout << "Book ID : ";

    while (!(cin >> bookID)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    int bIndex = findBookIndex3(bookID);

    if (bIndex == -1) {
        cout << "Book not found.\n";
        return;
    }

    if (books[bIndex].availableCopies > 0) {
        cout << "\nThis book still has available copies.\n";
        cout << "Reservation is only allowed for fully borrowed books.\n";
        return;
    }

    string memberID = getMemberID3();

    int qIndex = getOrCreateQueueIndex3(bookID);

    for (int i = 0; i < bookQueues[qIndex].waitCount; i++) {

        if (bookQueues[qIndex].waitList[i] == memberID) {

            cout << "This member has already reserved this book.\n";
            return;
        }
    }

    if (bookQueues[qIndex].waitCount >= MAX_WAITLIST) {

        cout << "Waiting list is full for this book.\n";
        return;
    }

    bookQueues[qIndex].waitList[bookQueues[qIndex].waitCount] = memberID;
    bookQueues[qIndex].waitCount++;

    cout << "\nReservation Successful.\n";
    cout << "Position in waiting list : " << bookQueues[qIndex].waitCount << endl;
}


// TRACK DUE DATE

void trackDueDate() {

    if (borrowRecords.empty()) {
        cout << "No borrowing records available.\n";
        return;
    }

    int choice = -1;

    cout << "\n====== Track Due Date ======\n";
    cout << "1. Search by Member ID\n";
    cout << "2. Search by Book ID\n";
    cout << "Enter choice : ";

    if (!(cin >> choice)) {

        cout << "Invalid choice. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        return;
    }

    bool found = false;

    if (choice == 1) {

        string memberID = getMemberID3();

        for (BorrowRecord r : borrowRecords) {

            if (r.memberID == memberID && r.returned == false) {

                found = true;

                cout << "\nBook ID   : " << r.bookID << endl;
                cout << "Member ID : " << r.memberID << endl;
                cout << "Due Date  : " << r.dueDate << endl;
                cout << "----------------------------------------\n";
            }
        }
    }
    else if (choice == 2) {

        int bookID;

        cout << "Book ID : ";

        while (!(cin >> bookID)) {

            cout << "Invalid Book ID. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            cout << "Book ID : ";
        }

        for (BorrowRecord r : borrowRecords) {

            if (r.bookID == bookID && r.returned == false) {

                found = true;

                cout << "\nBook ID   : " << r.bookID << endl;
                cout << "Member ID : " << r.memberID << endl;
                cout << "Due Date  : " << r.dueDate << endl;
                cout << "----------------------------------------\n";
            }
        }
    }
    else {
        cout << "Invalid choice.\n";
        return;
    }

    if (!found) {
        cout << "No active loan found.\n";
    }
}


// RENEW LOAN

void renewLoan() {

    int bookID;

    cout << "Book ID : ";

    while (!(cin >> bookID)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    bool foundRecord = false;

    for (BorrowRecord& r : borrowRecords) {

        if (r.bookID == bookID && r.returned == false) {

            foundRecord = true;

            if (r.renewed) {

                cout << "This loan has already been renewed once.\n";
                return;
            }

            int qIndex = findQueueIndex3(bookID);

            if (qIndex != -1 && bookQueues[qIndex].waitCount > 0) {

                cout << "This book has been reserved by another member.\n";
                cout << "Renewal is not allowed.\n";
                return;
            }

            cin.ignore();

            string newDueDate;

            cout << "Current Due Date : " << r.dueDate << endl;
            cout << "New Due Date (DD/MM/YYYY) : ";
            getline(cin, newDueDate);

            while (!validDate3(newDueDate)) {

                cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                getline(cin, newDueDate);
            }

            newDueDate = formatDate3(newDueDate);

            while (dateToNumber3(newDueDate) <= dateToNumber3(r.dueDate)) {

                cout << "New due date must be after the current due date.\n";

                cout << "New Due Date (DD/MM/YYYY) : ";
                getline(cin, newDueDate);

                while (!validDate3(newDueDate)) {

                    cout << "Invalid date format. Please enter again (DD/MM/YYYY): ";
                    getline(cin, newDueDate);
                }

                newDueDate = formatDate3(newDueDate);
            }

            r.dueDate = newDueDate;
            r.renewed = true;

            saveBorrowFile();

            cout << "\nLoan Renewed Successfully.\n";
            cout << "New Due Date : " << r.dueDate << endl;

            return;
        }
    }

    if (!foundRecord) {
        cout << "No active loan found for this book.\n";
    }
}


// CHECK AND LIST OVERDUE BOOKS

void checkOverdueBooks() {

    if (borrowRecords.empty()) {
        cout << "No borrowing records available.\n";
        return;
    }

    bool found = false;
    int todayDate = todayDateNumber3();

    cout << left
        << setw(10) << "Book ID"
        << setw(14) << "Member ID"
        << setw(14) << "Due Date"
        << setw(12) << "Days Late"
        << endl;

    cout << string(50, '-') << endl;

    for (BorrowRecord r : borrowRecords) {

        if (r.returned == false) {

            int daysOverdue = todayDate - dateToNumber3(r.dueDate);

            if (daysOverdue > 0) {

                found = true;

                cout << left
                    << setw(10) << r.bookID
                    << setw(14) << r.memberID
                    << setw(14) << r.dueDate
                    << setw(12) << daysOverdue
                    << endl;
            }
        }
    }

    if (!found) {
        cout << "\nNo overdue books.\n";
    }
}


// LOANS DUE TODAY (AUTOMATED NOTIFICATION)

void notifyDueToday3() {

    if (borrowRecords.empty()) {
        return;
    }

    int todayDate = todayDateNumber3();
    bool found = false;

    for (BorrowRecord r : borrowRecords) {

        if (r.returned == false && dateToNumber3(r.dueDate) == todayDate) {

            if (!found) {
                cout << "\n***************************************************************************\n";
                cout << "  NOTICE: " << TODAY_DAY << "/" << TODAY_MONTH << "/" << TODAY_YEAR
                    << " - The following loan(s) are due to be settled TODAY\n";
                cout << "***************************************************************************\n";
                cout << left
                    << setw(10) << "Book ID"
                    << setw(14) << "Member ID"
                    << setw(14) << "Due Date"
                    << endl;
                cout << string(38, '-') << endl;

                found = true;
            }

            cout << left
                << setw(10) << r.bookID
                << setw(14) << r.memberID
                << setw(14) << r.dueDate
                << endl;
        }
    }

    if (found) {
        cout << "***************************************************************************\n";
    }
    else {
        cout << "\nNo loans are due today.\n";
    }
}


// MODULE 3 MENU

void module3Menu() {

    loadFile();
    loadBorrowFile();

    // Automated notification: staff see this the moment they open the module, without needing to remember to check for it themselves.
    notifyDueToday3();

    int choice = -1;

    do {
        cout << "\n==============================\n";
        cout << " RESERVATION & LOAN MANAGEMENT\n";
        cout << "==============================\n";
        cout << "1. Reserve a Book\n";
        cout << "2. Track Due Date\n";
        cout << "3. Renew Loan\n";
        cout << "4. Check Overdue Books\n";
        cout << "5. View Loans Due Today\n";
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
            reserveBook();
            break;

        case 2:
            trackDueDate();
            break;

        case 3:
            renewLoan();
            break;

        case 4:
            checkOverdueBooks();
            break;

        case 5:
            notifyDueToday3();
            break;

        case 0:
            break;

        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}