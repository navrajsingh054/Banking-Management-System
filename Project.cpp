#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std;

class Account {
protected:
    int accountNumber;
    string name;
    string accountType;
    double balance;

public:
    Account() {
        accountNumber = 0;
        name = "";
        accountType = "";
        balance = 0.0;
    }

    Account(int accNo, string accName, string accType, double bal) {
        accountNumber = accNo;
        name = accName;
        accountType = accType;
        balance = bal;
    }

    virtual void createAccount() {
        cout << "\nEnter Account Number: ";
        cin >> accountNumber;

        cin.ignore();
        cout << "Enter Customer Name: ";
        getline(cin, name);

        cout << "Enter Account Type (Savings/Current): ";
        getline(cin, accountType);

        cout << "Enter Initial Deposit Amount: Rs. ";
        cin >> balance;
    }

    virtual void displayAccount() const {
        cout << "\n====================================";
        cout << "\nAccount Number : " << accountNumber;
        cout << "\nCustomer Name  : " << name;
        cout << "\nAccount Type   : " << accountType;
        cout << "\nBalance        : Rs. " << fixed << setprecision(2) << balance;
        cout << "\n====================================\n";
    }

    virtual void deposit(double amount) {
        balance += amount;
        cout << "\nRs. " << amount << " deposited successfully." << endl;
        cout << "Updated Balance: Rs. " << balance << endl;
    }

    virtual bool withdraw(double amount) {
        if (amount > balance) {
            cout << "\nInsufficient balance!" << endl;
            return false;
        }

        balance -= amount;
        cout << "\nRs. " << amount << " withdrawn successfully." << endl;
        cout << "Remaining Balance: Rs. " << balance << endl;
        return true;
    }

    int getAccountNumber() const {
        return accountNumber;
    }

    double getBalance() const {
        return balance;
    }

    string toFileString() const {
        stringstream ss;
        ss << accountNumber << "|"
           << name << "|"
           << accountType << "|"
           << balance;
        return ss.str();
    }

    void fromFileString(string line) {
        stringstream ss(line);
        string temp;

        getline(ss, temp, '|');
        accountNumber = stoi(temp);

        getline(ss, name, '|');
        getline(ss, accountType, '|');

        getline(ss, temp, '|');
        balance = stod(temp);
    }
};

class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount() : Account() {
        interestRate = 4.0;
    }

    void displaySavingsInfo() {
        cout << "Interest Rate: " << interestRate << "%" << endl;
    }
};

class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount() : Account() {
        overdraftLimit = 5000.0;
    }

    void displayCurrentInfo() {
        cout << "Overdraft Limit: Rs. " << overdraftLimit << endl;
    }
};

class Transaction {
public:
    static void saveTransaction(int accNo, string type, double amount) {
        ofstream file("transactions.txt", ios::app);

        if (!file) {
            cout << "Error opening transaction file!" << endl;
            return;
        }

        file << "Account No: " << accNo
             << " | Transaction: " << type
             << " | Amount: Rs. " << amount << endl;

        file.close();
    }
};

class BankManagementSystem {
private:
    vector<Account> accounts;

public:
    BankManagementSystem() {
        loadAccountsFromFile();
    }

    void loadAccountsFromFile() {
        accounts.clear();

        ifstream file("accounts.txt");

        if (!file) {
            return;
        }

        string line;

        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            Account acc;
            acc.fromFileString(line);
            accounts.push_back(acc);
        }

        file.close();
    }

    void saveAccountsToFile() {
        ofstream file("accounts.txt");

        if (!file) {
            cout << "Error opening accounts file!" << endl;
            return;
        }

        for (const auto &acc : accounts) {
            file << acc.toFileString() << endl;
        }

        file.close();
    }

    bool accountExists(int accNo) {
        for (const auto &acc : accounts) {
            if (acc.getAccountNumber() == accNo) {
                return true;
            }
        }

        return false;
    }

    int findAccountIndex(int accNo) {
        for (int i = 0; i < accounts.size(); i++) {
            if (accounts[i].getAccountNumber() == accNo) {
                return i;
            }
        }

        return -1;
    }

    void createNewAccount() {
        Account acc;
        acc.createAccount();

        if (accountExists(acc.getAccountNumber())) {
            cout << "\nAccount number already exists!" << endl;
            return;
        }

        accounts.push_back(acc);
        saveAccountsToFile();

        cout << "\nAccount created successfully!" << endl;
    }

    void displayAllAccounts() {
        if (accounts.empty()) {
            cout << "\nNo accounts found!" << endl;
            return;
        }

        cout << "\n=========== ALL ACCOUNTS ===========\n";

        for (const auto &acc : accounts) {
            acc.displayAccount();
        }
    }

    void searchAccount() {
        int accNo;
        cout << "\nEnter Account Number to Search: ";
        cin >> accNo;

        int index = findAccountIndex(accNo);

        if (index == -1) {
            cout << "\nAccount not found!" << endl;
            return;
        }

        accounts[index].displayAccount();
    }

    void depositMoney() {
        int accNo;
        double amount;

        cout << "\nEnter Account Number: ";
        cin >> accNo;

        int index = findAccountIndex(accNo);

        if (index == -1) {
            cout << "\nAccount not found!" << endl;
            return;
        }

        cout << "Enter Amount to Deposit: Rs. ";
        cin >> amount;

        if (amount <= 0) {
            cout << "\nInvalid deposit amount!" << endl;
            return;
        }

        accounts[index].deposit(amount);
        saveAccountsToFile();

        Transaction::saveTransaction(accNo, "Deposit", amount);
    }

    void withdrawMoney() {
        int accNo;
        double amount;

        cout << "\nEnter Account Number: ";
        cin >> accNo;

        int index = findAccountIndex(accNo);

        if (index == -1) {
            cout << "\nAccount not found!" << endl;
            return;
        }

        cout << "Enter Amount to Withdraw: Rs. ";
        cin >> amount;

        if (amount <= 0) {
            cout << "\nInvalid withdrawal amount!" << endl;
            return;
        }

        bool success = accounts[index].withdraw(amount);

        if (success) {
            saveAccountsToFile();
            Transaction::saveTransaction(accNo, "Withdrawal", amount);
        }
    }

    void balanceInquiry() {
        int accNo;

        cout << "\nEnter Account Number: ";
        cin >> accNo;

        int index = findAccountIndex(accNo);

        if (index == -1) {
            cout << "\nAccount not found!" << endl;
            return;
        }

        cout << "\nCurrent Balance: Rs. "
             << fixed << setprecision(2)
             << accounts[index].getBalance() << endl;
    }

    void deleteAccount() {
        int accNo;

        cout << "\nEnter Account Number to Delete: ";
        cin >> accNo;

        int index = findAccountIndex(accNo);

        if (index == -1) {
            cout << "\nAccount not found!" << endl;
            return;
        }

        accounts.erase(accounts.begin() + index);
        saveAccountsToFile();

        cout << "\nAccount deleted successfully!" << endl;
    }

    void showTransactions() {
        ifstream file("transactions.txt");

        if (!file) {
            cout << "\nNo transactions found!" << endl;
            return;
        }

        cout << "\n========== TRANSACTION HISTORY ==========" << endl;

        string line;

        while (getline(file, line)) {
            cout << line << endl;
        }

        file.close();
    }

    void displayMenu() {
        cout << "\n=========================================";
        cout << "\n     BANKING MANAGEMENT SYSTEM";
        cout << "\n=========================================";
        cout << "\n1. Create New Account";
        cout << "\n2. Display All Accounts";
        cout << "\n3. Search Account";
        cout << "\n4. Deposit Money";
        cout << "\n5. Withdraw Money";
        cout << "\n6. Balance Inquiry";
        cout << "\n7. Delete Account";
        cout << "\n8. Show Transaction History";
        cout << "\n9. Exit";
        cout << "\n=========================================";
        cout << "\nEnter Your Choice: ";
    }

    void run() {
        int choice;

        do {
            displayMenu();
            cin >> choice;

            switch (choice) {
                case 1:
                    createNewAccount();
                    break;

                case 2:
                    displayAllAccounts();
                    break;

                case 3:
                    searchAccount();
                    break;

                case 4:
                    depositMoney();
                    break;

                case 5:
                    withdrawMoney();
                    break;

                case 6:
                    balanceInquiry();
                    break;

                case 7:
                    deleteAccount();
                    break;

                case 8:
                    showTransactions();
                    break;

                case 9:
                    cout << "\nThank you for using Banking Management System!" << endl;
                    break;

                default:
                    cout << "\nInvalid choice! Please try again." << endl;
            }

        } while (choice != 9);
    }
};

int main() {
    BankManagementSystem bank;
    bank.run();

    return 0;
}
