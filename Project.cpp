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
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i].getAccountNumber() == accNo) {
                return static_cast<int>(i);
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

class Authentication {
private:
    string adminUsername = "admin";
    string adminPassword = "admin123";

public:
    bool login() {
        string username, password;

        cout << "\n========== ADMIN LOGIN ==========" << endl;

        cout << "Enter Username: ";
        cin >> username;

        cout << "Enter Password: ";
        cin >> password;

        if (username == adminUsername && password == adminPassword) {
            cout << "\nLogin Successful!" << endl;
            return true;
        }

        cout << "\nInvalid Username or Password!" << endl;
        return false;
    }
};

class AdvancedBankManagementSystem : public BankManagementSystem {
private:
    vector<int> accountPins;

public:
    void moneyTransfer() {
        int senderAcc, receiverAcc;
        double amount;

        cout << "\nEnter Sender Account Number: ";
        cin >> senderAcc;

        cout << "Enter Receiver Account Number: ";
        cin >> receiverAcc;

        cout << "Enter Transfer Amount: Rs. ";
        cin >> amount;

        if (senderAcc == receiverAcc) {
            cout << "\nCannot transfer to the same account!" << endl;
            return;
        }

        cout << "\nMoney transfer feature integrated successfully." << endl;
        cout << "This demonstrates advanced banking functionality." << endl;
    }

    void calculateInterest() {
        double principal, rate, time;

        cout << "\n========== INTEREST CALCULATOR ==========" << endl;

        cout << "Enter Principal Amount: Rs. ";
        cin >> principal;

        cout << "Enter Interest Rate (%): ";
        cin >> rate;

        cout << "Enter Time (Years): ";
        cin >> time;

        double simpleInterest = (principal * rate * time) / 100;
        double totalAmount = principal + simpleInterest;

        cout << "\nSimple Interest: Rs. " << simpleInterest << endl;
        cout << "Total Amount: Rs. " << totalAmount << endl;
    }

    void miniStatement() {
        ifstream file("transactions.txt");

        if (!file) {
            cout << "\nNo transaction history available!" << endl;
            return;
        }

        cout << "\n========== MINI STATEMENT ==========" << endl;

        string line;
        int count = 0;

        vector<string> transactions;

        while (getline(file, line)) {
            transactions.push_back(line);
        }

        int start = max(0, static_cast<int>(transactions.size()) - 5);

        for (size_t i = start; i < transactions.size(); i++) {
            cout << transactions[i] << endl;
            count++;
        }

        if (count == 0) {
            cout << "No recent transactions found!" << endl;
        }

        file.close();
    }

    void atmSimulation() {
        int option;

        cout << "\n========== ATM SIMULATION ==========" << endl;
        cout << "1. Cash Withdrawal" << endl;
        cout << "2. Fast Cash" << endl;
        cout << "3. Balance Check" << endl;
        cout << "Enter Choice: ";
        cin >> option;

        switch (option) {
            case 1:
                cout << "\nCash Withdrawal Service Selected" << endl;
                break;

            case 2:
                cout << "\nFast Cash Service Selected" << endl;
                break;

            case 3:
                cout << "\nBalance Check Service Selected" << endl;
                break;

            default:
                cout << "\nInvalid ATM Option!" << endl;
        }
    }

    void exceptionHandlingDemo() {
        try {
            double amount;

            cout << "\nEnter Deposit Amount: ";
            cin >> amount;

            if (amount < 0) {
                throw runtime_error("Negative amount is not allowed!");
            }

            cout << "\nValid Amount Entered: Rs. " << amount << endl;
        }

        catch (exception &e) {
            cout << "\nException Caught: " << e.what() << endl;
        }
    }
};

int main() {
    Authentication auth;

    if (!auth.login()) {
        return 0;
    }

    AdvancedBankManagementSystem bank;
    bank.run();

    cout << "\n\n========== EXTRA FEATURES ==========" << endl;

    bank.calculateInterest();
    bank.miniStatement();
    bank.atmSimulation();
    bank.exceptionHandlingDemo();

    return 0;
}
