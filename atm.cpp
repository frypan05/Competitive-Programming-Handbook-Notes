#include <conio.h>
#include <iostream>
#include <string>
using namespace std;

/* Mini Project - ATM
   -> Check Balance
   -> Cash Withdraw
   -> User Details
   -> Update Mobile Number
*/

class atm{
  private:
    long int account_No;
    string name;
    int PIN;
    double balance;
    string mobile_No;
  
  public:  
    //Setter function is being used now
    void setData(long int account_No_a, string name_a, int PIN_a, double balance_a, string mobile_No_a){
      account_No = account_No_a;
      name = name_a;
      PIN = PIN_a;
      balance = balance_a;
      mobile_No_a = mobile_No_a;
    }
    //getAccountNo function is returning the user's account no.
    long int getAccountNo(){
      return account_No;
    }

    //getName function is returning the user's name
    string getName(){
      return name;
    }

    int getPIN(){
      return PIN;
    }

    double getBalance(){
      return balance;
    }

    string getMobileNo(){
      return mobile_No;
    }

    
    //setMobile function is updating the user mobile no.
    void setMobile(string mob_prev, string mob_new){
      if (mob_prev == mobile_No){
        mobile_No = mob_new;
        cout << endl << "Successfully Updated the Mobile no.";
        _getch();
      }
    }
    //cashWithdrawal function is used to withdraw from the atm
    void cashWithDraw(int amount_a){
      if (amount_a > 0 && amount_a < balance) {
        balance -= amount_a;
        cout << endl << "Please collect your cash";
        cout << endl << "Available Balance: " << balance;
        _getch();
      }
      else {
        cout << endl << "Invalid Input or Insufficient Balance";
        _getch();
      }
    }
};

int main(){
  int choice =0, enterPIN;
  long int enterAccountNo;

  system("cls");

  atm user1;
  user1.setData(1234567, "Tim", 1111, 45000.90, "0987654321");


  do{
    system("cls");
    cout << endl << "** Welcome to the ATM **" << endl;
    cout << endl << "Enter Your Account No. ";
    cin >> enterAccountNo;

    cout << endl << "Enter PIN ";
    cin >> enterPIN;

    if ((enterAccountNo == user1.getAccountNo()) && (enterPIN == user1.getPIN())){
      do{
        int amount = 0;
        string oldMobileNo, newMobileNo;

        system("cls");
        cout << endl << "** Welcome to the ATM **" << endl;
        cout << endl << "Select Options ";
        cout << endl << "1. Check Balance";
        cout << endl << "2. Cash Withdraw";
        cout << endl << "3. Show User Details";
        cout << endl << "4. Update Mobile No.";
        cout << endl << "5. Exit"; << endl;
        cin >> choice;

        switch (choice){
          case 1:
            cout << endl << "Your balance is: " << user1.getBalance();
            _getch();
            break;

          case 2:
            cout << endl << "Enter the amount: ";
            cin >> amount;
            user1.cashWithdrawal(amount);

            break;

          case 3:
            cout << endl << "** User Details are: ";
            cout << endl << "-> Account No: " << user1.getAccountNo();
            cout << endl << "-> Name:       " << user1.getName();
            cout << endl << "-> Balance:    " << user1.getBalance();
            cout << endl << "-> Mobile No.  " << user1.getMobileNo();

            _getch();
            break;

          case 4:
            cout << endl << Enter Old mobile No
        }
      }
    }
  }
}
