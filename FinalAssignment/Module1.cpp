#include "Globals.h"
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void loadBorrowFile();

// TEXT HELPERS

string trim(const string& text) {

    int start = 0;
    int end = static_cast<int>(text.length()) - 1;

    while (start <= end && isspace(text[start])) {
        start++;
    }

    while (end >= start && isspace(text[end])) {
        end--;
    }

    if (start > end) {
        return "";
    }

    return text.substr(start, end - start + 1);
}

string toLowerText(string text) {

    for (char& ch : text) {
        ch = static_cast<char>(tolower(ch));
    }

    return text;
}

string toUpperText(string text) {

    for (char& ch : text) {
        ch = static_cast<char>(toupper(ch));
    }

    return text;
}

bool containsIgnoreCase(const string& source, const string& keyword) {
    return toLowerText(source).find(toLowerText(keyword)) != string::npos;
}

void enterScreen() {
    string temp;
    cout << "\nPress Enter to continue...";
    getline(cin, temp);
}

// INPUT HELPERS

bool parseIntStrict(const string& input, int& value) {
    stringstream ss(trim(input));
    ss >> value;

    if (ss.fail()) {
        return false;
    }

    char extra;
    return !(ss >> extra);
}

int readInt(const string& prompt, int minValue, int maxValue) {
    string input;
    int value;

    while (true) {
        cout << prompt;

        if (!getline(cin, input)) {
            exit(0);
        }

        if (parseIntStrict(input, value) && value >= minValue && value <= maxValue) {
            return value;
        }

        cout << "Invalid input. Please enter in number from "
            << minValue << " to " << maxValue << ".\n";
    }
}

string readRequiredLine(const string& prompt) {
    string input;

    while (true) {
        cout << prompt;

        if (!getline(cin, input)) {
            exit(0);
        }

        input = trim(input);

        if (!input.empty()) {
            return input;
        }

        cout << "Input cannot be empty.\n";
    }
}

// VALIDATION

bool isValidName(const string& name) {

    if (trim(name).empty()) {
        return false;
    }

    bool hasLetter = false;

    for (char ch : name) {
        if (isalpha(ch)) {
            hasLetter = true;
        }
        else if (ch != ' ') {
            return false;
        }
    }

    return hasLetter;
}

const int MIN_PHONE_LENGTH = 10;
const int MAX_PHONE_LENGTH = 11;

bool isValidPhone(const string& phone) {

    if (phone.length() < MIN_PHONE_LENGTH || phone.length() > MAX_PHONE_LENGTH) {
        return false;
    }

    if (phone.substr(0, 2) != "01") {
        return false;
    }

    for (char ch : phone) {
        if (!isdigit(ch)) {
            return false;
        }
    }

    return true;
}

bool isDuplicatePhone(const string& phone, int ignoreIndex = -1) {

    string targetPhone = trim(phone);

    for (int i = 0; i < static_cast<int>(members.size()); i++) {
        if (i != ignoreIndex && members[i].phone == targetPhone) {
            return true;
        }
    }

    return false;
}

const int STUDENT_ID_LENGTH = 7;

bool isValidStudentId(const string& studentId) {

    if (studentId.length() != STUDENT_ID_LENGTH) {
        return false;
    }

    for (char ch : studentId) {
        if (!isdigit(ch)) {
            return false;
        }
    }

    return true;
}

string StaffId(int number) {

    stringstream ss;
    ss << "S" << setw(3) << setfill('0') << number;
    return ss.str();
}

// =======================================================
// MEMBER / LOAN LOOKUP HELPERS
// (read from the global members / borrow records / nextStaffNo)
// =======================================================

int findMemberIndexById(const string& memberId) {

    string target = toUpperText(trim(memberId));

    for (int i = 0; i < static_cast<int>(members.size()); i++) {
        if (toUpperText(members[i].memberId) == target) {
            return i;
        }
    }

    return -1;
}

bool hasActiveLoan(const string& memberId) {

    string target = toUpperText(trim(memberId));

    for (const LoanRecord& loan : loans) {
        if (toUpperText(loan.memberId) == target && !loan.returned) {
            return true;
        }
    }

    for (const BorrowRecord& record : borrowRecords) {
        if (toUpperText(record.memberID) == target && !record.returned) {
            return true;
        }
    }

    return false;
}

int countActiveLoans(const string& memberId) {

    int count = 0;
    string target = toUpperText(trim(memberId));

    for (const LoanRecord& loan : loans) {

        if (toUpperText(loan.memberId) == target && !loan.returned) {
            count++;
        }
    }

    for (const BorrowRecord& record : borrowRecords) {

        if (toUpperText(record.memberID) == target && !record.returned) {
            count++;
        }
    }

    return count;
}

void updateLoanMemberId(const string& oldId, const string& newId) {

    for (LoanRecord& loan : loans) {

        if (toUpperText(loan.memberId) == toUpperText(oldId)) {
            loan.memberId = newId;
        }
    }

    for (BorrowRecord& record : borrowRecords) {

        if (toUpperText(record.memberID) == toUpperText(oldId)) {
            record.memberID = newId;
        }
    }
}

string chooseMembershipType() {

    cout << "\nMembership Type\n";
    cout << "1. Student\n";
    cout << "2. Staff\n";

    int choice = readInt("Select membership type: ", 1, 2);

    if (choice == 1) {
        return "Student";
    }

    return "Staff";
}

string readStudentId(int ignoreIndex = -1) {

    string studentId;

    while (true) {
        studentId = readRequiredLine("Enter student ID (7 digits): ");

        if (!isValidStudentId(studentId)) {
            cout << "Invalid student ID. Student ID must be exactly 7 digits.\n";
            continue;
        }

        int existingIndex = findMemberIndexById(studentId);

        if (existingIndex != -1 && existingIndex != ignoreIndex) {
            cout << "Duplicate student ID. This member already exists.\n";
        }
        else {
            return studentId;
        }
    }
}

string generateStaffId() {

    string staffId;

    do {
        staffId = StaffId(nextStaffNo);
        nextStaffNo++;

    } while (findMemberIndexById(staffId) != -1);

    return staffId;
}

// ==============================
// DISPLAY
// ==============================

void displayMemberHeader(bool showLoans = false) {

    cout << "==============================================================================\n";
    cout << left << setw(12) << "ID"
        << setw(26) << "Name"
        << setw(14) << "Phone"
        << setw(14) << "Type"
        << setw(10) << "Status";

    if (showLoans) {
        cout << setw(8) << "Loans";
    }

    cout << endl;
    cout << "==============================================================================\n";
}

void displayMemberRow(const Member& member, int activeLoans = -1) {

    cout << left << setw(12) << member.memberId
        << setw(26) << member.name.substr(0, 25)
        << setw(14) << member.phone
        << setw(14) << member.membershipType
        << setw(10) << (member.active ? "Active" : "Inactive");

    if (activeLoans >= 0) {
        cout << setw(8) << activeLoans;
    }

    cout << endl;
}

void displayMemberSelectionList(const vector<int>& matchedIndexes) {

    cout << "==============================================================================\n";
    cout << left << setw(6) << "No."
        << setw(12) << "ID"
        << setw(26) << "Name"
        << setw(14) << "Phone"
        << setw(14) << "Type"
        << setw(10) << "Status"
        << setw(8) << "Loans" << endl;

    cout << "==============================================================================\n";

    for (int i = 0; i < static_cast<int>(matchedIndexes.size()); i++) {

        const Member& member = members[matchedIndexes[i]];

        cout << left << setw(6) << (i + 1)
            << setw(12) << member.memberId
            << setw(26) << member.name.substr(0, 25)
            << setw(14) << member.phone
            << setw(14) << member.membershipType
            << setw(10) << (member.active ? "Active" : "Inactive")
            << setw(8) << countActiveLoans(member.memberId) << endl;
    }
}

int selectMemberForAction(const string& actionName) {

    if (members.empty()) {

        cout << "No member records available.\n";
        return -1;
    }

    cout << "\nFind member to " << actionName << "\n";
    cout << "==============================================================================\n";
    cout << "1. Search by Member ID\n";
    cout << "2. Search by name\n";
    cout << "3. Search by phone number\n";
    cout << "4. Display all members\n";
    cout << "0. Cancel\n";

    int choice = readInt("Select search option(0-4): ", 0, 4);

    if (choice == 0) {

        cout << "Action cancelled.\n";
        return -1;
    }

    string keyword;

    if (choice == 1) {
        keyword = readRequiredLine("Enter Member ID or part of ID: ");
    }
    else if (choice == 2) {
        keyword = readRequiredLine("Enter member name keyword: ");
    }
    else if (choice == 3) {
        keyword = readRequiredLine("Enter phone number or part of phone number: ");
    }

    vector<int> matchedIndexes;

    for (int i = 0; i < static_cast<int>(members.size()); i++) {
        bool matched = false;

        if (choice == 1) {
            matched = containsIgnoreCase(members[i].memberId, keyword);
        }
        else if (choice == 2) {
            matched = containsIgnoreCase(members[i].name, keyword);
        }
        else if (choice == 3) {
            matched = members[i].phone.find(keyword) != string::npos;
        }
        else {
            matched = true;
        }

        if (matched) {
            matchedIndexes.push_back(i);
        }
    }

    if (matchedIndexes.empty()) {
        cout << "No matching member found.\n";
        return -1;
    }

    displayMemberSelectionList(matchedIndexes);

    int selectedNo = readInt("Select member number (0 to cancel): ",
        0,
        static_cast<int>(matchedIndexes.size()));

    if (selectedNo == 0) {
        cout << "Action cancelled.\n";
        return -1;
    }

    return matchedIndexes[selectedNo - 1];
}

// ==============================
// MEMBER ACTIONS
// ==============================

void registerMember() {

    cout << "\nREGISTER NEW MEMBER\n";
    cout << "==============================================================================\n";

    string name;

    do {
        name = readRequiredLine("Enter member name: ");

        if (!isValidName(name)) {
            cout << "Invalid name. Use letters and spaces only.\n";
        }
    } while (!isValidName(name));

    string phone;

    do {
        phone = readRequiredLine("Enter phone number (10-11 digits): ");

        if (!isValidPhone(phone)) {
            cout << "Invalid phone number.Start with '01', length 10 to 11.(Only digits allowed)\n";
        }
        else if (isDuplicatePhone(phone)) {
            cout << "Duplicate phone number. This phone number is already registered.\n";
        }
    } while (!isValidPhone(phone) || isDuplicatePhone(phone));

    string membershipType = chooseMembershipType();
    string memberId;

    if (membershipType == "Student") {
        memberId = readStudentId();
    }
    else {
        memberId = generateStaffId();
    }

    Member newMember;
    newMember.memberId = memberId;
    newMember.name = name;
    newMember.phone = phone;
    newMember.membershipType = membershipType;
    newMember.active = true;

    members.push_back(newMember);

    cout << "\nMember registered successfully.\n";

    if (membershipType == "Student") {
        cout << "Student ID recorded: " << memberId << endl;
    }
    else {
        cout << "Generated Staff ID: " << memberId << endl;
    }
}

void updateMembershipType(int memberIndex) {

    string oldId = members[memberIndex].memberId;
    string oldType = members[memberIndex].membershipType;
    string newType = chooseMembershipType();

    if (newType == oldType) {
        cout << "Membership type is unchanged.\n";
        return;
    }

    if (newType == "Student") {
        members[memberIndex].memberId = readStudentId(memberIndex);
    }
    else {
        members[memberIndex].memberId = generateStaffId();
    }

    members[memberIndex].membershipType = newType;
    updateLoanMemberId(oldId, members[memberIndex].memberId);

    cout << "Membership type updated from " << oldType << " to " << newType << ".\n";
    cout << "New Member ID: " << members[memberIndex].memberId << endl;
}

void updateMember() {
    cout << "\nUPDATE MEMBER DETAILS\n";
    cout << "==============================================================================\n";

    int index = selectMemberForAction("update");

    if (index == -1) {
        return;
    }

    cout << "\nSelected member:\n";
    displayMemberHeader();
    displayMemberRow(members[index]);

    int choice;

    do {
        cout << "\nUpdate Options\n";
        cout << "1. Update name\n";
        cout << "2. Update phone number\n";
        cout << "3. Update membership type\n";
        cout << "4. Reactivate member\n";
        cout << "0. Back\n";

        choice = readInt("Select option(0-4): ", 0, 4);

        switch (choice) {
        case 1:
        {
            string name;

            do {
                name = readRequiredLine("Enter new name: ");

                if (!isValidName(name)) {
                    cout << "Invalid name. Use letters and spaces only.\n";
                }
            } while (!isValidName(name));

            members[index].name = name;
            cout << "Member name updated.\n";
            break;
        }
        case 2:
        {
            string phone;

            do {
                phone = readRequiredLine("Enter new phone number (10-11 digits): ");

                if (!isValidPhone(phone)) {
                    cout << "Invalid phone number.It must start with '01' (10-11 digits).\n";
                }
                else if (isDuplicatePhone(phone, index)) {
                    cout << "Duplicate phone number. This phone number is already used by another member.\n";
                }
            } while (!isValidPhone(phone) || isDuplicatePhone(phone, index));

            members[index].phone = phone;
            cout << "Phone number updated.\n";
            break;
        }
        case 3:
            updateMembershipType(index);
            break;
        case 4:
            if (members[index].active) {
                cout << "Member is already active.\n";
            }
            else {
                members[index].active = true;
                cout << "Member reactivated.\n";
            }
            break;
        case 0:
            break;
        default:
            cout << "Invalid option.Only 0 - 4 Choices Are Valid.\n";
        }
    } while (choice != 0);
}

void searchMember() {

    cout << "\nSEARCH MEMBER RECORD\n";
    cout << "==============================================================================\n";

    if (members.empty()) {
        cout << "No member records available.\n";
        return;
    }

    string keyword = readRequiredLine("Enter ID, name, or phone keyword: ");
    bool found = false;

    displayMemberHeader(true);

    for (const Member& member : members) {
        if (containsIgnoreCase(member.memberId, keyword) ||
            containsIgnoreCase(member.name, keyword) ||
            member.phone.find(keyword) != string::npos)
        {
            displayMemberRow(member, countActiveLoans(member.memberId));
            found = true;
        }
    }

    if (!found) {
        cout << "No matching member found.\n";
    }
}

void displayAllMembers() {

    cout << "\nMEMBER LIST\n";

    if (members.empty()) {
        cout << "No member records available.\n";
        return;
    }

    displayMemberHeader();

    for (const Member& member : members) {
        displayMemberRow(member);
    }
}

void deactivateMember() {

    cout << "\nDEACTIVATE MEMBER\n";
    cout << "==============================================================================\n";

    loadBorrowFile();

    int index = selectMemberForAction("deactivate");

    if (index == -1) {
        return;
    }

    cout << "\nSelected member:\n";
    displayMemberHeader();
    displayMemberRow(members[index]);

    if (!members[index].active) {
        cout << "This member is already inactive.\n";
        return;
    }

    if (hasActiveLoan(members[index].memberId)) {
        cout << "\nCannot deactivate this member because there are active borrowed books.\n";
        cout << "Please return all borrowed books before deactivating the member.\n";
        return;
    }

    int confirm = readInt("Confirm deactivate member? (1 = Yes, 0 = No): ", 0, 1);

    if (confirm == 1) {
        members[index].active = false;
        cout << "Member deactivated successfully.\n";
    }
    else {
        cout << "Action cancelled.\n";
    }
}

void SampleData() {

    members.push_back({ "2501234", "Lim Xiao Mei ", "0123456789", "Student", true });
    members.push_back({ "S001", "Tan Mei Ling", "01123456789", "Staff", true });
    members.push_back({ "2599999", "Ahmad Hakim", "0198765432", "Student", false });


    nextStaffNo = 2;
}

void introScreen() {

    cout << "==============================================================================\n";
    cout << "                   Hello, library staff.\n";
    cout << "    This member management module helps you register, update,\n";
    cout << "       search, view, and deactivate library member records.\n";
    cout << "==============================================================================\n";
}

// ==============================
// MODULE 1 MENU
// ==============================

void module1Menu() {

    introScreen();

    loadBorrowFile();

    int choice;

    do {
        cout << "\nMEMBER MANAGEMENT MENU\n";
        cout << "==============================================================================\n";
        cout << "1. Register new member\n";
        cout << "2. Update member details\n";
        cout << "3. Search member record\n";
        cout << "4. Display all members\n";
        cout << "5. Deactivate member\n";
        cout << "0. Back\n";

        choice = readInt("Select option(0-5): ", 0, 5);

        switch (choice) {
        case 1:
            registerMember();
            enterScreen();
            break;
        case 2:
            updateMember();
            enterScreen();
            break;
        case 3:
            searchMember();
            enterScreen();
            break;
        case 4:
            displayAllMembers();
            enterScreen();
            break;
        case 5:
            deactivateMember();
            enterScreen();
            break;
        case 0:
            cout << "\nReturning to main menu.\n";
            break;
        default:
            cout << "Invalid menu choice.Only 0 - 5 Choices Are Valid.\n";
        }

    } while (choice != 0);
}