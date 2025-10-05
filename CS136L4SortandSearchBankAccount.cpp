#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace std;

enum class SortField { FIRST_NAME = 1, LAST_NAME, ID, AVAILABLE, PRESENT, SSN, EMAIL, BACK };
enum class MainMenu { SORT = 1, SEARCH, QUIT };

class BankAccount {
private:
    string firstName, lastName, email, ssn;
    int accountID;
    double availableBalance, presentBalance;

public:
    BankAccount(string fn, string ln, string em, string ss, int id, double avail, double present)
        : firstName(fn), lastName(ln), email(em), ssn(ss), accountID(id),
        availableBalance(avail), presentBalance(present) {
    }

    void print() const {
        cout << "ID: " << accountID << ", Name: " << firstName << " " << lastName
            << ", Email: " << email << ", SSN: " << ssn
            << ", Available: $" << availableBalance << ", Present: $" << presentBalance << endl;
    }

    static string toLower(const string& s) {
        string result = s;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    string getEmail() const { return email; }
    string getSSN() const { return ssn; }
    int getID() const { return accountID; }
    double getAvailableBalance() const { return availableBalance; }
    double getPresentBalance() const { return presentBalance; }
};

class Bank {
private:
    BankAccount** accounts;
    int numAccounts;

public:
    Bank() : accounts(nullptr), numAccounts(0) {}
    ~Bank() {
        for (int i = 0; i < numAccounts; ++i) delete accounts[i];
        delete[] accounts;
    }

    void loadAccounts(const string& filename) {
        cout << "Looking for file: " << filename << endl;
        ifstream file(filename);
        if (!file) throw runtime_error("Error opening file.");

        string fn, ln, em, ss;
        int id;
        double avail, present;
        while (getline(file, fn, ',') &&
            getline(file, ln, ',') &&
            getline(file, em, ',') &&
            getline(file, ss, ',') &&
            file >> id &&
            file.ignore() &&
            file >> avail &&
            file.ignore() &&
            file >> present) {
            file.ignore(); // skip newline
            addAccount(new BankAccount(fn, ln, em, ss, id, avail, present));
        }
    }

    void addAccount(BankAccount* acc) {
        BankAccount** temp = new BankAccount * [numAccounts + 1];
        for (int i = 0; i < numAccounts; ++i) temp[i] = accounts[i];
        temp[numAccounts++] = acc;
        delete[] accounts;
        accounts = temp;
    }

    void printAccounts() const {
        for (int i = 0; i < numAccounts; ++i) accounts[i]->print();
    }

    void sortAccounts(SortField field) {
        for (int i = 0; i < numAccounts - 1; ++i) {
            for (int j = 0; j < numAccounts - i - 1; ++j) {
                bool toSwap = false;
                switch (field) {
                case SortField::FIRST_NAME:
                    toSwap = BankAccount::toLower(accounts[j]->getFirstName()) >
                        BankAccount::toLower(accounts[j + 1]->getFirstName());
                    break;
                case SortField::LAST_NAME:
                    toSwap = BankAccount::toLower(accounts[j]->getLastName()) >
                        BankAccount::toLower(accounts[j + 1]->getLastName());
                    break;
                case SortField::ID:
                    toSwap = accounts[j]->getID() > accounts[j + 1]->getID();
                    break;
                case SortField::AVAILABLE:
                    toSwap = accounts[j]->getAvailableBalance() > accounts[j + 1]->getAvailableBalance();
                    break;
                case SortField::PRESENT:
                    toSwap = accounts[j]->getPresentBalance() > accounts[j + 1]->getPresentBalance();
                    break;
                case SortField::SSN:
                    toSwap = BankAccount::toLower(accounts[j]->getSSN()) >
                        BankAccount::toLower(accounts[j + 1]->getSSN());
                    break;
                case SortField::EMAIL:
                    toSwap = BankAccount::toLower(accounts[j]->getEmail()) >
                        BankAccount::toLower(accounts[j + 1]->getEmail());
                    break;
                default: break;
                }
                if (toSwap) {
                    BankAccount* temp = accounts[j];
                    accounts[j] = accounts[j + 1];
                    accounts[j + 1] = temp;
                }
            }
        }
    }

    void sortMenu() {
        int choice;
        do {
            cout << "\nSort by:\n1. First Name\n2. Last Name\n3. ID\n4. Available Balance\n5. Present Balance\n6. SSN\n7. Email\n8. Back\nChoice: ";
            cin >> choice;
            SortField field = static_cast<SortField>(choice);
            switch (field) {
            case SortField::BACK: cout << "Returning to main menu...\n"; break;
            default:
                sortAccounts(field);
                printAccounts();
                break;
            }
        } while (choice != 8);
    }

    void searchRecords(const string& query) {
        string q = BankAccount::toLower(query);
        bool found = false;
        for (int i = 0; i < numAccounts; ++i) {
            string fn = BankAccount::toLower(accounts[i]->getFirstName());
            string ln = BankAccount::toLower(accounts[i]->getLastName());
            string em = BankAccount::toLower(accounts[i]->getEmail());
            string ss = BankAccount::toLower(accounts[i]->getSSN());
            string id = to_string(accounts[i]->getID());

            if (fn.find(q) != string::npos || ln.find(q) != string::npos ||
                em.find(q) != string::npos || ss.find(q) != string::npos ||
                id.find(q) != string::npos) {
                accounts[i]->print();
                found = true;
            }
        }
        if (!found) cout << "No matching records found.\n";
    }

    void searchByBalance(double target, double percent) {
        double lower = target - (target * percent / 100.0);
        double upper = target + (target * percent / 100.0);
        cout << "Searching for balances between $" << lower << " and $" << upper << "\n";

        bool found = false;
        for (int i = 0; i < numAccounts; ++i) {
            double bal = accounts[i]->getAvailableBalance();
            if (bal >= lower && bal <= upper) {
                accounts[i]->print();
                found = true;
            }
        }
        if (!found) cout << "No records found in the specified range.\n";
    }
};

int main() {
    Bank bank;
    try {
        bank.loadAccounts("accounts.txt");
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    int choice;
    do {
        cout << "\nMain Menu:\n1. Sort\n2. Search\n3. Quit\nChoice: ";
        cin >> choice;
        switch (static_cast<MainMenu>(choice)) {
        case MainMenu::SORT:
            bank.sortMenu();
            break;
        case MainMenu::SEARCH: {
            cout << "Enter search term (ID, name, email, SSN): ";
            string term;
            cin >> term;
            bank.searchRecords(term);

            cout << "Search by balance range? (y/n): ";
            char balChoice;
            cin >> balChoice;
            if (tolower(balChoice) == 'y') {
                double target, percent;
                cout << "Enter target balance: ";
                cin >> target;
                cout << "Enter percent range: ";
                cin >> percent;
                bank.searchByBalance(target, percent);
            }
            break;
        }
        case MainMenu::QUIT:
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
            break;
        }
    } while (choice != 3);

    return 0;
}