#include "Globals.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

// ==============================================================
// These are defined in Module2.cpp and load/refresh the shared
// books / borrowRecords vectors from file. Forward declared here
// so Module 4 has up-to-date data even if staff opens this module
// before visiting Module 2.
// ==============================================================

void loadFile();
void loadBorrowFile();

// Defined in Module1.cpp - "Press Enter to continue..." pause,
// reused here so output isn't wiped by clearScreen() before it's read.
void enterScreen();

// This module reads menu choices with cin >> (not getline), which
// leaves a trailing newline in the buffer. Calling enterScreen()
// directly after that would consume the leftover newline instantly
// instead of actually waiting for the user - so clear it first.
static void pauseScreen4() {
    cin.clear();
    cin.ignore(1000, '\n');
    enterScreen();
}

const double DAILY_FINE_RATE = 0.50;

// A payment made against a returned, overdue BorrowRecord.
// Kept local to this module - no other module needs to read it.
struct FinePayment {
    string memberID;
    int bookID = 0;
    string bookTitle;
    string dueDate;
    string returnDate;
    double fineAmount = 0.0;
    double amountPaid = 0.0;
    double change = 0.0;
    string paymentDate;
};

static vector<FinePayment> finePayments;

// MEMBER ID VALIDATION
// (same student/staff ID rules as Module 2 and Module 3)

static bool validStudentID4(string id) {

    if (id.length() != 7)
        return false;

    for (char c : id) {
        if (!isdigit(c))
            return false;
    }

    return true;
}

static bool validStaffID4(string id) {

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

static string getMemberID4() {
    string id;

    while (true) {
        cout << "Member ID (Student 7 digits / Staff S001) : ";
        cin >> id;

        if (validStudentID4(id) || validStaffID4(id)) {
            return id;
        }
        else {
            cout << "Invalid Member ID.\n";
        }
    }
}

// EXISTENCE CHECKS
// getMemberID4()/bookID input only validate FORMAT (7 digits, or
// "S" + 3 digits) - they don't check the ID actually belongs to a
// real member or book. These checks let processFinePayment() give
// a specific "not found" message instead of a generic "no record"
// message when the ID is simply made up or mistyped.

static bool memberExists4(const string& memberID) {

    for (const Member& m : members) {
        if (m.memberId == memberID) {
            return true;
        }
    }

    return false;
}

static bool bookExists4(int bookID) {

    for (const Book& b : books) {
        if (b.id == bookID) {
            return true;
        }
    }

    return false;
}

// DATE TO NUMBER (DD/MM/YYYY) - for comparing dates already stored
// in a BorrowRecord, which Module 2 has already validated on entry.

static int dateToNumber4(string date) {

    int day, month, year;
    char slash1, slash2;

    stringstream ss(date);

    ss >> day >> slash1 >> month >> slash2 >> year;

    return year * 365 + month * 30 + day;
}

static int todayDateNumber4() {
    return TODAY_YEAR * 365 + TODAY_MONTH * 30 + TODAY_DAY;
}

// FIND BOOK TITLE BY ID

static string findBookTitle4(int bookID) {

    for (Book b : books) {

        if (b.id == bookID) {
            return b.title;
        }
    }

    return "Unknown Title";
}

// FIND UNPAID, RETURNED BORROW RECORD FOR A MEMBER + BOOK

static int findUnpaidReturnedRecord4(const string& memberID, int bookID) {

    // Search newest -> oldest. A member can borrow/return the same
    // book multiple times, so scanning forward from index 0 would
    // stop at the OLDEST matching returned record (which may have
    // already been settled with no fine) instead of the loan that
    // was just returned in Module 2. Going backwards ensures the
    // most recently returned record is picked up first.

    for (int i = static_cast<int>(borrowRecords.size()) - 1; i >= 0; i--) {

        BorrowRecord r = borrowRecords[i];

        if (r.memberID != memberID || r.bookID != bookID || r.returned == false) {
            continue;
        }

        bool alreadyPaid = false;

        for (FinePayment p : finePayments) {

            if (p.memberID == r.memberID &&
                p.bookID == r.bookID &&
                p.returnDate == r.returnDate) {

                alreadyPaid = true;
                break;
            }
        }

        if (!alreadyPaid) {
            return i;
        }
    }

    return -1;
}

// FORMAT TODAY AS DD/MM/YYYY

static string todayAsString4() {

    stringstream ss;

    ss << TODAY_DAY << "/" << TODAY_MONTH << "/" << TODAY_YEAR;

    return ss.str();
}

// CALCULATE DAYS LATE AND FINE OWED
// Split into their own functions (matching calculateLoanDays() in
// Module 2) rather than inlined, so each does one clear job and
// returns a value the caller uses.

static int calculateDaysLate4(const string& dueDate, const string& returnDate) {

    int daysLate = dateToNumber4(returnDate) - dateToNumber4(dueDate);

    return (daysLate > 0) ? daysLate : 0;
}

static double calculateFine4(int daysLate) {
    return daysLate * DAILY_FINE_RATE;
}

// Fills in a FinePayment record via reference, so the caller gets
// the completed record back without processFinePayment() needing
// to build and return a whole struct itself.

static void buildPaymentRecord4(FinePayment& payment, const string& memberID, int bookID,
    const BorrowRecord& record, double fineOwed, double amountPaid) {

    payment.memberID = memberID;
    payment.bookID = bookID;
    payment.bookTitle = findBookTitle4(bookID);
    payment.dueDate = record.dueDate;
    payment.returnDate = record.returnDate;
    payment.fineAmount = fineOwed;
    payment.amountPaid = amountPaid;
    payment.change = amountPaid - fineOwed;
    payment.paymentDate = todayAsString4();
}

// ==============================================================
// FUNCTION 1 & 2: Calculate fine and process payment
// ==============================================================

void processFinePayment() {

    cout << "\n====== Process Fine Payment ======\n";

    if (borrowRecords.empty()) {
        cout << "No borrowing records available.\n";
        return;
    }

    string memberID = getMemberID4();

    int bookID;

    cout << "Book ID : ";

    while (!(cin >> bookID)) {

        cout << "Invalid Book ID. Please enter a number.\n";

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Book ID : ";
    }

    if (!memberExists4(memberID)) {
        cout << "Member not found.\n";
        return;
    }

    if (!bookExists4(bookID)) {
        cout << "Book not found.\n";
        return;
    }

    int recordIndex = findUnpaidReturnedRecord4(memberID, bookID);

    if (recordIndex == -1) {
        cout << "No unpaid, returned borrowing record found for this member and book.\n";
        return;
    }

    const BorrowRecord& record = borrowRecords[recordIndex];

    int daysLate = calculateDaysLate4(record.dueDate, record.returnDate);
    double fineOwed = calculateFine4(daysLate);

    cout << fixed << setprecision(2);
    cout << "\nBook Title : " << findBookTitle4(bookID) << endl;
    cout << "Due Date   : " << record.dueDate << endl;
    cout << "Return Date: " << record.returnDate << endl;
    cout << "Days Late  : " << daysLate << endl;
    cout << "Fine Owed  : RM" << fineOwed << endl;

    if (fineOwed == 0) {
        cout << "No fine owed. Book was returned on or before the due date.\n";
        return;
    }

    double amountPaid;

    while (true) {

        cout << "Enter payment amount : RM";

        if (!(cin >> amountPaid)) {

            cout << "Invalid input. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            continue;
        }

        if (amountPaid < 0) {
            cout << "Invalid Payment Amount. Amount cannot be negative.\n";
            continue;
        }

        if (amountPaid < fineOwed) {
            cout << "Insufficient Payment. Amount must be at least RM"
                << fineOwed << ".\n";
            continue;
        }

        break;
    }

    FinePayment payment;
    buildPaymentRecord4(payment, memberID, bookID, record, fineOwed, amountPaid);

    finePayments.push_back(payment);

    cout << "\n---------------- RECEIPT ----------------\n";
    cout << "Member ID     : " << memberID << endl;
    cout << "Book Title    : " << payment.bookTitle << endl;
    cout << "Days Late     : " << daysLate << endl;
    cout << "Fine Amount   : RM" << fineOwed << endl;
    cout << "Amount Paid   : RM" << amountPaid << endl;
    cout << "Change        : RM" << payment.change << endl;
    cout << "Payment Date  : " << payment.paymentDate << endl;
    cout << "-------------------------------------------\n";
}

// ==============================================================
// FUNCTION 3: Track payment history per member
// ==============================================================

void viewPaymentHistory() {

    cout << "\n====== Payment History ======\n";

    if (finePayments.empty()) {
        cout << "No payment records available.\n";
        return;
    }

    string memberID = getMemberID4();
    bool found = false;

    cout << left
        << setw(14) << "Book ID"
        << setw(26) << "Book Title"
        << setw(12) << "Fine(RM)"
        << setw(12) << "Paid(RM)"
        << setw(14) << "Date"
        << endl;

    cout << string(78, '-') << endl;

    for (FinePayment p : finePayments) {

        if (p.memberID == memberID) {

            found = true;

            cout << left
                << setw(14) << p.bookID
                << setw(26) << p.bookTitle.substr(0, 25)
                << setw(12) << fixed << setprecision(2) << p.fineAmount
                << setw(12) << p.amountPaid
                << setw(14) << p.paymentDate
                << endl;
        }
    }

    if (!found) {
        cout << "No payment history found for this member.\n";
    }
}

// ==============================================================
// FUNCTION 4: Generate reports
// ==============================================================

void mostBorrowedBooksReport() {

    cout << "\n====== Most Borrowed Books Report ======\n";

    if (borrowRecords.empty()) {
        cout << "No borrowing records available.\n";
        return;
    }

    vector<int> bookIDs;
    vector<int> counts;

    for (BorrowRecord r : borrowRecords) {

        bool matched = false;

        for (int i = 0; i < static_cast<int>(bookIDs.size()); i++) {

            if (bookIDs[i] == r.bookID) {
                counts[i]++;
                matched = true;
                break;
            }
        }

        if (!matched) {
            bookIDs.push_back(r.bookID);
            counts.push_back(1);
        }
    }

    // Simple bubble sort - highest borrow count first
    for (int i = 0; i < static_cast<int>(counts.size()) - 1; i++) {

        for (int j = 0; j < static_cast<int>(counts.size()) - 1 - i; j++) {

            if (counts[j] < counts[j + 1]) {

                swap(counts[j], counts[j + 1]);
                swap(bookIDs[j], bookIDs[j + 1]);
            }
        }
    }

    cout << left
        << setw(30) << "Book Title"
        << setw(10) << "Times"
        << endl;

    cout << string(40, '-') << endl;

    for (int i = 0; i < static_cast<int>(bookIDs.size()); i++) {

        cout << left
            << setw(30) << findBookTitle4(bookIDs[i]).substr(0, 28)
            << setw(10) << counts[i]
            << endl;
    }
}

void overdueSummaryReport() {

    cout << "\n====== Overdue & Financial Summary Report ======\n";

    int overdueCount = 0;
    int todayDate = todayDateNumber4();

    for (BorrowRecord r : borrowRecords) {

        if (r.returned == false && dateToNumber4(r.dueDate) < todayDate) {
            overdueCount++;
        }
    }

    double totalFinesCollected = 0;

    for (FinePayment p : finePayments) {
        totalFinesCollected += p.fineAmount;
    }

    int activeMembers = 0;

    for (Member m : members) {

        if (m.active) {
            activeMembers++;
        }
    }

    cout << fixed << setprecision(2);
    cout << "Currently Overdue Books : " << overdueCount << endl;
    cout << "Total Fines Collected   : RM" << totalFinesCollected << endl;
    cout << "Total Active Members    : " << activeMembers << endl;

    if (members.empty()) {
        cout << "(Note: Member Management has not been opened this session,\n"
            << " so active member count may be incomplete.)\n";
    }
}

void reportsMenu4() {

    int choice;

    do {
        cout << "\n====== Reports Menu ======\n";
        cout << "1. Most Borrowed Books Report\n";
        cout << "2. Overdue & Financial Summary Report\n";
        cout << "0. Back\n";
        cout << "Choice : ";

        // Keep re-prompting for just the choice (not the whole menu)
        // until we get a valid integer.
        while (!(cin >> choice)) {

            cout << "Invalid choice. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            cout << "Choice : ";
        }

        switch (choice) {

        case 1:
            mostBorrowedBooksReport();
            pauseScreen4();
            break;

        case 2:
            overdueSummaryReport();
            pauseScreen4();
            break;

        case 0:
            break;

        default:
            cout << "Invalid choice.\n";
            pauseScreen4();
        }

    } while (choice != 0);
}

// ==============================================================
// MODULE 4 MENU
// ==============================================================

void module4Menu() {

    loadFile();
    loadBorrowFile();

    int choice;

    do {
        cout << "\n==============================\n";
        cout << " FINE & PAYMENT PROCESSING\n";
        cout << "==============================\n";
        cout << "1. Process Fine Payment\n";
        cout << "2. View Payment History\n";
        cout << "3. Reports\n";
        cout << "0. Back\n";
        cout << "==============================\n";
        cout << "Choice : ";

        // Keep re-prompting for just the choice (not the whole menu)
        // until we get a valid integer.
        while (!(cin >> choice)) {

            cout << "Invalid choice. Please enter a number.\n";

            cin.clear();
            cin.ignore(1000, '\n');

            cout << "Choice : ";
        }

        switch (choice) {

        case 1:
            processFinePayment();
            pauseScreen4();
            break;

        case 2:
            viewPaymentHistory();
            pauseScreen4();
            break;

        case 3:
            reportsMenu4();
            break;

        case 0:
            break;

        default:
            cout << "Invalid choice.\n";
            pauseScreen4();
        }

    } while (choice != 0);
}