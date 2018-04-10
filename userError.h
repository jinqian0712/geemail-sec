/*This is a header file, userError.h
  This is a file which declare exceptions
*/
#include <stdexcept>
#include <string>

using namespace std;

class userSelectionError: public exception {
    private:
    unsigned char userSelection;
    public:
    userSelectionError(unsigned char userInput){
        userSelection = userInput;
    }
    const char* what(){
        cout << "INFO* try to enter a wrong value  " 
             << userSelection << endl; 
        return "ERROR* got invalid input. please use S/s or R/r.";
    }
};

class invalidUsernameError: public exception {
   private:
   size_t usernameLength;
   public:
   invalidUsernameError(string username){
       usernameLength = username.size();
   }
   const char* what(){
       cout << endl
            << "INFO* try to use invalid username length "
            << usernameLength << endl;
        return "ERROR* wrong username length, length must between 4 to 20.";
   }
};

class invalidPasswordError: public exception {
   private:
   size_t passwordLength;
   public:
   invalidPasswordError(string password){
       passwordLength = password.size();
   }
   const char* what(){
       cout << endl
            << "INFO* try to use invalid password length "
            << passwordLength << endl;
        return "ERROR* wrong password length, length must between 4 to 16.";
   }
};

class userExistError: public exception {
   public:
   userExistError(string userName){
       cout << endl
            << "INFO* the username "
            << userName 
            << " already taken." << endl;
   }
   const char* what(){
        return "ERROR* username already taken, try another one.";
   }
};

class selectionError: public exception {
   public:
   selectionError(void){
       cout << endl
            << "INFO* some critical error hapens ";
   }
   const char* what(){
        return "ERROR* process accur a critical error.";
   }
};

class invalidSelectError: public exception {
   public:
   invalidSelectError(unsigned int num){
       cout << endl
            << "INFO* try to input an invalid number " << num << endl;
   }
   const char* what(){
        return "ERROR* illegal input, try again.";
   }
};

class userNotExistError: public exception {
   public:
   userNotExistError(void){
       cout << endl
            << "INFO* receiver not found." << endl;
   }
   const char* what(){
        return "ERROR* make sure you enter a right user.";
   }
};

class inputSizeError: public exception {
   public:
   inputSizeError(void){
       cout << endl
            << "INFO* invalid input." << endl;
   }
   const char* what(){
        return "ERROR* input size overload, try again.";
   }
};