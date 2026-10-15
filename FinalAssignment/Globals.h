#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>

using namespace std;

// ============================================================
// STRUCTS (Module 1 - Member Management)
// ============================================================

struct Member {
    string memberId;
    string name;
    string phone;
    string membershipType;
    bool active;
};

struct LoanRecord {
    string memberId;
    string bookTitle;
    bool returned;
};

// ============================================================
// STRUCTS (Module 2 - Book Catalog & Borrowing)
// ============================================================

struct Book {
    int id;
    string title;
    string author;
    string isbn;
    string category;
    int totalCopies;
    int availableCopies;
};

struct BorrowRecord {
    string memberID;
    int bookID;
    string borrowDate;
    string dueDate;
    string returnDate;
    bool returned;
    bool renewed;
};

// ============================================================
// STRUCTS (Module 3 - Reservation & Loan Schedule Management)
// ============================================================

const int MAX_WAITLIST = 10;

struct BookQueue {
    int bookID;
    string waitList[MAX_WAITLIST];
    int waitCount;
};

// ============================================================
// STRUCTS (Module 4 - Fine & Payment Processing with Reporting)
// ============================================================

struct FineRecord {
    string memberId;
    string bookTitle;
    int dueDay, dueMonth, dueYear;
    int returnDay, returnMonth, returnYear;
    double fineAmount;
    double amountPaid;
    double change;
    bool paid;
};

// ============================================================
// SHARED CONSTANTS
// ============================================================

// Current system date used across modules for date comparisons
// (e.g. checking past dates, calculating overdue days).
// Update this in one place if the reference date ever changes.
extern const int TODAY_DAY;
extern const int TODAY_MONTH;
extern const int TODAY_YEAR;

// ============================================================
// GLOBAL VARIABLES (Module 1)
// ============================================================

extern vector<Member> members;
extern vector<LoanRecord> loans;
extern int nextStaffNo;

// ============================================================
// GLOBAL VARIABLES (Module 2)
// ============================================================

extern vector<Book> books;
extern vector<BorrowRecord> borrowRecords;

extern const string bookFile;
extern const string borrowFile;

// ============================================================
// GLOBAL VARIABLES (Module 3)
// ============================================================

extern vector<BookQueue> bookQueues;

// ============================================================
// GLOBAL VARIABLES (Module 4)
// ============================================================

extern vector<FineRecord> fines;

#endif