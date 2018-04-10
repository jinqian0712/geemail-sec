#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>
#include <climits>
#include "hashgen.h"
#include "userError.h"
#include "geemaildb.h"

using namespace std;

struct Userinfo {
    public:
    string userName;
    string passHash;
};

unsigned char welcomePage(void){
    
    unsigned char userSelection;
    
    cout << "**********************************" 
         << endl
         << "*      WELCOME TO GEEMAIL        *"
         << endl
         << "**********************************"
         << endl;
    
    cout << endl << "R/r for Sign_up" 
         << endl << "S/s for Sign_in" 
         << endl << "Please Enter:";
    cin >> userSelection;
    switch(userSelection){
        case 'R':
        case 'r':
        case 'S':
        case 's':return userSelection;
                 break;
        default: throw userSelectionError(userSelection);
    }

}

void HideStdinKeystrokes()
{
    termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* disable echo */
    tty.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void ShowStdinKeystrokes()
{
   termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* reenable echo */
    tty.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

string getPassword(void){
    
    string password;
    
    HideStdinKeystrokes();
    cin >> password;
    ShowStdinKeystrokes();
    
    return password;
    
}

bool checkLogin(string username, string password, Userinfo* user){
    
    string passSalt;
    string passwordHash;
    bool loginSuccess=false;
    bool isExist;
    
    if(isUserExist(username)){
        
        passSalt = getUserSalt(username,loginSuccess,0);
        passwordHash = generatePasswordHash(username,password,passSalt);
        loginSuccess = isPasshashMatched(username,passwordHash,false,0);
        if(!loginSuccess){
            cout << endl << "ERROR* password unmatched, please try again." << endl;
        }else{
            user->userName = username;
            user->passHash = passwordHash;
        }
    }else{
        cout << endl << "ERROR* no username matches, please try again." << endl;
    }
    
    return loginSuccess;
    
}

bool userAuth(unsigned char* userSelection, bool loginSuccess, Userinfo* user){
    
    string username;
    string userpassword;
    string passwordHash;
    
    if(((*userSelection)=='R')||((*userSelection) == 'r')){
        try{
            cout << "SIGN_UP" <<endl;
            cout << "Enter Username(username length must between 4 to 20):";
            cin >> username;
            if((username.size()<4)||(username.size()>20)){
                throw invalidUsernameError(username);
            }
            if(isUserExist(username)){
               throw userExistError(username);
            }
            cout << "Enter Password(password length must between 4 to 16):";
            userpassword = getPassword();
            if((userpassword.size()<4)||(userpassword.size()>16)){
                throw invalidPasswordError(userpassword);
            }
            cout << endl
                 << "\nYOUR ACCOUNT " << username << "@geemail.jinqian "
                 << "ALREADY SET.\n"
                 << endl;
            
            bool insertSuccess = false;
            insertSuccess = insertIntoDatabase(username,userpassword);
            
            /*If Sign_up Success, automatically transfer to login page. */
            try{
                if(insertSuccess){
                    (*userSelection) = 'S';
                    insertSuccess = false;
                }
                if(*userSelection!='S'){
                    throw selectionError();
                }
            }catch(selectionError e){
                cout << e.what() << "\n" << endl;
            }
        }catch(invalidUsernameError e){
            cout << e.what() << "\n" << endl;
        }catch(userExistError e){
            cout << e.what() << "\n" << endl;
        }catch(invalidPasswordError e){
            cout << e.what() << "\n" << endl;
        }
        
    }else{
        try{
            cout << "LOGIN" <<endl;
            cout << "USERNAME:";
            cin >> username;
            cin.clear();
            if((username.size()<4)||(username.size()>20)){
                throw invalidUsernameError(username);
            }
            cout << "PASSWORD:";
            userpassword = getPassword();
            if((userpassword.size()<4)||(userpassword.size()>16)){
                throw invalidPasswordError(userpassword);
            }
            loginSuccess = checkLogin(username,userpassword,user);
        }catch(invalidUsernameError e){
            cout << e.what() << "\n" << endl;
        }catch(invalidPasswordError e){
            cout << e.what() << "\n" << endl;
        }
    }
    
    return loginSuccess;
    
}

void userPage(bool loginSuccess, bool invalidProcess, unsigned char userSelection, Userinfo user){
    
    unsigned int max_num;
    while(loginSuccess){
        
        max_num = showMessagePage(user.userName);
        invalidProcess = true;
        while(invalidProcess){
            cout << endl
                 << "(R/r for read, S/s for send)"
                 << endl
                 << "Enter:";
            try{
                cin >> userSelection;
                switch(userSelection){
                    case 'R':
                    case 'r':
                    case 'S':
                    case 's':break;
                    default: throw userSelectionError(userSelection);
                }
                invalidProcess = false;
                cin.clear();
            }catch(userSelectionError e){
                cout << "\n" << e.what() << "\n" << endl;
            }
        }
        
        //READ MOD
        invalidProcess = true;
        unsigned int messageNum = 0;
        bool illegalInput;
        bool cinFail;
        bool readAuth;
        
        while(invalidProcess){
            if((userSelection == 'R')||(userSelection=='r')){
                try{
                    illegalInput = true;
                    while(illegalInput){
                        cinFail = true;
                        while(cinFail){
                            cout << endl << "READ MOD" << endl;
                            cout << "\nSelect message number which you want to read." << endl;
                            cout << "Enter:";
                            cin >> messageNum;
                            if(cin.fail()){
                                cin.clear();
                                cin.ignore(INT_MAX, '\n');
                                cout << "\nERROR* an illegal input, try again." <<endl;
                            }else{
                                cinFail = false;
                            }
                        }
                        if((messageNum<0)||(messageNum>max_num)){
                            throw invalidSelectError(messageNum);
                        }
                        illegalInput = false;
                    }
                    
                    string passPhrase;
                    string passphraseHash;
                    cout << "Enter PassPhrase:";
                    passPhrase = getPassword();
                    if((passPhrase.size()<4)||(passPhrase.size()>16)){
                        throw invalidPasswordError(passPhrase);
                    }
                    string passphraseSalt;
                    passphraseSalt = getUserSalt(user.userName,loginSuccess,messageNum);
                    
                    passphraseHash = generatePasswordHash(user.userName, passPhrase, passphraseSalt);
                    
                    readAuth = false;
                    readAuth = isPasshashMatched(user.userName,passphraseHash,loginSuccess,messageNum);
                    
                    if(readAuth){
                        invalidProcess = false;
                        readMessage(user.userName,messageNum);
                    }else{
                        cout << endl << "INFO* Wrong Passphrase, Permission Deny." << endl;
                    }
                    unsigned char pause;
                    HideStdinKeystrokes();
                    cin >> pause;
                    ShowStdinKeystrokes();
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    
                    invalidProcess = false;
                    
                }catch(invalidPasswordError e){
                    cout << e.what() << "\n" << endl;
                }catch(invalidSelectError e){
                    cout << e.what() << "\n" << endl;
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                }
            }else{
                string receiver;
                illegalInput = true;
                try{
                    while(illegalInput){
                        cout << endl << "SEND MOD" << endl;
                        cout << "Enter receiver(username length must between 4 to 20):";
                        cin >> receiver;
                        if((receiver.size()<4)||(receiver.size()>20)){
                            throw invalidUsernameError(receiver);
                        }
                        if(!isUserExist(receiver)){
                            throw userNotExistError();
                        }
                        string setPassPhrase;
                        cout << "Enter Password(password length must between 4 to 16):";
                        setPassPhrase = getPassword();
                        if((setPassPhrase.size()<4)||(setPassPhrase.size()>16)){
                            throw invalidPasswordError(setPassPhrase);
                        }
                        cout << "\nMake Sure You Remerber Your Passphrase " << setPassPhrase << endl;
                        bool sendSuccess;
                        sendSuccess = false;
                        sendSuccess = sendMessage(user.userName,receiver,setPassPhrase);
                        
                        if(sendSuccess){
                            cout << endl << "Message send success!\n" << endl;
                            invalidProcess = false;
                            illegalInput = false;
                        }else{
                            cout << endl << "Fail to send!\n" << endl;
                        }
                        
                    }
                }catch(invalidUsernameError e){
                    cout << e.what() << "\n" << endl;
                }catch(invalidPasswordError e){
                    cout << e.what() << "\n" << endl;
                }catch(userNotExistError e){
                    cout << e.what() << "\n" << endl;
                }
            }
        }
    }
}

int main(int argc, char *argv[]){
    if(argc != 1){
        cout << "ERROR* system loading failed!" << endl;
        return 1;
    }
    
    bool loginSuccess = false;
    
    //Print WelcomePage, acquire user's selection;
    bool invalidProcess = true;
    unsigned char userSelection;

    while(invalidProcess){
        try{
            userSelection = welcomePage();
            invalidProcess = false;
        }catch(userSelectionError e){
            cout << "\n" << e.what() << "\n" << endl;
            cin.clear();
            cin.ignore(INT_MAX, '\n');
        }
    }
    
    //user login or sign_up
    Userinfo user;
    
    while(!loginSuccess){
        loginSuccess = userAuth(&userSelection,loginSuccess,&user);
    }
    
    cout << endl << "\nYOU ALREADY LOGIN, WELCOME USER " << user.userName 
         << "@geemail.jinqian\n"
         << endl;
    
    userSelection = 'e';      // reset after login;
    invalidProcess = true;
    
    //UserPage
    userPage(loginSuccess,invalidProcess,userSelection,user);
    
    
    return 0;
}