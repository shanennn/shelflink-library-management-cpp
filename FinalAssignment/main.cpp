#include <iostream>
#include "Globals.h"
#include "Module1.h"
#include "Module2.h"
#include "Module3.h"
#include "Module4.h"

using namespace std;

int main()
{
    int choice = -1;

    SampleData();

    do
    {
        cout << "\n=========================================\n";
        cout << "      LIBRARY MANAGEMENT SYSTEM\n";
        cout << "=========================================\n";
        cout << "1. Member Management\n";
        cout << "2. Book Catalog & Borrowing\n";
        cout << "3. Reservation & Loan Management\n";
        cout << "4. Fine & Payment Processing\n";
        cout << "0. Exit\n";
        cout << "=========================================\n";
        cout << "Enter choice : ";
        if (!(cin >> choice)) {
            if (cin.eof()) {
                break;
            }
            cout << "\nInvalid input. Please enter a number from 0 to 4.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
            continue;
        }

        switch (choice)
        {
        case 1:
            cin.ignore(1000, '\n');
            module1Menu();
            break;

        case 2:
            module2Menu();
            break;

        case 3:
            module3Menu();
            break;

        case 4:
            cin.ignore(1000, '\n');
            module4Menu();
            break;

        case 0:
            cout << "\nThank you.\n";
            break;

        default:
            cout << "\nInvalid choice. Please enter a number from 0 to 4.\n";
        }

    } while (choice != 0);

    return 0;
}