#include "Globals.h"
#include <ctime>

// ==============================
// SHARED CONSTANTS
// ==============================


static int getSystemDay() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    return localTime.tm_mday;
}

static int getSystemMonth() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    return localTime.tm_mon + 1;
}

static int getSystemYear() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    return localTime.tm_year + 1900;
}

const int TODAY_DAY = getSystemDay();
const int TODAY_MONTH = getSystemMonth();
const int TODAY_YEAR = getSystemYear();

// ==============================
// GLOBAL VARIABLES (Module 1)
// ==============================

vector<Member> members;
vector<LoanRecord> loans;
int nextStaffNo = 1;

// ==============================
// GLOBAL VARIABLES (Module 2)
// ==============================

vector<Book> books;
vector<BorrowRecord> borrowRecords;

const string bookFile = "books.txt";
const string borrowFile = "borrow.txt";

// ==============================
// GLOBAL VARIABLES (Module 3)
// ==============================

vector<BookQueue> bookQueues;

// ==============================
// GLOBAL VARIABLES (Module 4)
// ==============================

vector<FineRecord> fines;