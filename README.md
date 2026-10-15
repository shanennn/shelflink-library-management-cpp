# ShelfLink: Library Management System (C++)

A console-based library management system written in C++. Library staff can manage members, books, borrowing, reservations, fines and payments from one menu.

Built as a group project for AMCS2123 Systems and Programming Concepts (Diploma in Computer Science, TAR UMT).

## Main menu

```
=========================================
      LIBRARY MANAGEMENT SYSTEM
=========================================
1. Member Management
2. Book Catalog & Borrowing
3. Reservation & Loan Management
4. Fine & Payment Processing
0. Exit
=========================================
```

## Features

The system has four modules.

**1. Member Management**
- Register new members (student or staff)
- Update name, phone number or membership type, and reactivate a member
- Search by member ID, name or phone number
- Display all members
- Deactivate a member

**2. Book Catalog & Borrowing**
- Add, view, update and remove books
- Search by title, author or category
- Borrow and return books
- Book and borrowing records are saved in `books.txt` and `borrow.txt`

**3. Reservation & Loan Management**
- Reserve a book (waiting list of up to 10 members per book)
- Track due dates and renew loans
- Check overdue books
- View loans due today

**4. Fine & Payment Processing**
- Process fine payments for overdue books
- View payment history
- Reports: most borrowed books, and an overdue and financial summary

## Tech stack
- C++ (C++20)
- Structs, vectors, and file input/output (text files)
- Visual Studio (MSVC)

## How to run

You need Windows with Visual Studio and the "Desktop development with C++" workload.

1. Open `FinalAssignment/FinalAssignment.slnx` in Visual Studio.
2. Build and run (Ctrl + F5).

You can also run `build_finalassignment.bat` from the repository root. It builds `FinalAssignment.exe` into `FinalAssignment/x64/Debug`.

Run the program from the folder that contains `books.txt` and `borrow.txt`, so it can load the saved records.

## Project structure

```
FinalAssignment/FinalAssignment/
├── main.cpp            Main menu
├── Globals.h / .cpp    Shared structs, global data, and system date
├── Module1.h / .cpp    Member Management
├── Module2.h / .cpp    Book Catalog & Borrowing
├── Module3.h / .cpp    Reservation & Loan Management
├── Module4.h / .cpp    Fine & Payment Processing
├── books.txt           Saved book records
└── borrow.txt          Saved borrowing records
```

## My part
I was in charge of Module 4, **Fine & Payment Processing with Reporting**, which helps library staff handle overdue fines and payments.

- Fine calculation: finds the days overdue and charges RM0.50 per day
- Payment processing: checks the amount paid (rejects negative or insufficient amounts), calculates the change, and prints a receipt
- Payment history: staff can search by Member ID to see a member's payments (date, book title, amount)
- Reports: most borrowed books, and an overdue and financial summary (overdue books, total fines collected, number of active members)

## Team
Group project with my coursemates.
