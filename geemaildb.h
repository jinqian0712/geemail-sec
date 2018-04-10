/*This is a header file, geemaildb.h.
  It will be used for dealing with the geemail database.
*/

#include <string>
#include <vector>
using namespace std;

bool isUserExist(string username);
string getUserSalt(string username,bool loginSuccess,unsigned int num);
bool isPasshashMatched(string username,string passwordHash,bool loginSuccess,unsigned int num);
bool insertIntoDatabase(string username,string userpassword);
unsigned int showMessagePage(string username);
void readMessage(string username,unsigned int num);
bool sendMessage(string fromwho,string sendto,string passphrase);

struct Message{
    
    string num;
    string fromwho;
    string sendto;
    string date;
    string title;
    string passphrase;
    string salt;
    vector<string> mesg;
    
};

bool insertMessageColumn(Message* messg,string mesg);