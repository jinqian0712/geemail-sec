#include "geemaildb.h"
#include <cstring>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <climits>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <vector>
#include "userError.h"
#include "hashgen.h"


using namespace std;


static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    string* username = (string*)data;
    for(i=0; i<argc; i++){
        (*username) += argv[i];
    }
    
    return 0;
}

static int callbacknum(void *data, int argc, char **argv, char **azColName){
    
    int* num = (int*)data;
    istringstream (argv[0]) >> *num;
    return 0;
}

static int callbacknull(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    return 0;
}

static int callbackmesg(void *data, int argc, char **argv, char **azColName){
    int i;
    Message* m = (Message*)data;
    for(i=0; i<argc; i++){
         
         m->mesg.push_back(argv[i]);
    }
    return 0;
}

int sqlOperation(string sqlCode,string* user){
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    };
    
    string sql = sqlCode; 
    if(user == NULL){
        rc = sqlite3_exec(db, sql.c_str(), callbacknull, 0, &zErrMsg);
    }else{
        rc = sqlite3_exec(db, sql.c_str(), callback, user, &zErrMsg);
    }
    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }  
    sqlite3_close(db);
    return 0;
}

int sqlOperationNum(string sqlCode,int* num){
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    string sql = sqlCode; 
    
    rc = sqlite3_exec(db, sql.c_str(), callbacknum, num, &zErrMsg);
    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }  
    sqlite3_close(db);
    return 0;
}

bool isUserExist(string username){
    
    string sqlCode = "select username from useraccount where username = '"; 
    sqlCode += username;
    sqlCode += "';";
    bool isExist;
    string user;
    
    sqlOperation(sqlCode,&user);
    if(user != username){
        isExist = false;
    }else{
        isExist = true;
    }
    
    return isExist;
}

string getUserSalt(string username,bool loginSuccess,unsigned int num){
    
    string sqlCode; 
    stringstream s;
    s << num;
    if(loginSuccess == false){
        sqlCode = "select salt from useraccount where username = '"+ username + "';";
    }else{
        sqlCode = "select salt from message where sendto = '"+ username + "' and id =" + s.str()+";";
    }
    
    string userSalt;
    
    sqlOperation(sqlCode,&userSalt);
    
    return userSalt;
}

bool isPasshashMatched(string username,string passwordHash, bool loginSuccess, unsigned int num){
    
    bool isMatch = false;
    
    string sqlCode;
    string passHash;
    stringstream s;
    s << num;
    
    if(loginSuccess == false){
        sqlCode = "select passwordhash from useraccount where username = '"+ username + "';";
    }else{
        sqlCode = "select passphrase from message where sendto = '"+ username + "' and id =" +s.str()+";";
    }
    
    sqlOperation(sqlCode, &passHash);
    
    if(passHash == passwordHash){
        isMatch = true;
    }else{
        isMatch = false;
    }
    
    return isMatch;
    
}

bool insertIntoDatabase(string username,string userpassword){
    
    string salt;
    
    salt = userSaltGenertator();
    string passwordHash = generatePasswordHash(username, userpassword, salt);
    
    string sqlCode = "insert into useraccount (username,passwordhash,salt) values ";
    sqlCode += "('"+username+"','"+passwordHash+"','"+salt+"');";
    
    sqlOperation(sqlCode, NULL);
    
    bool inserted = true;
    
    return inserted;
}

unsigned int showMessagePage(string username){
    
    string sqlCode;
    int num;
    
    sqlCode = "select sendto from message where sendto ='"+ username +"';";
    string check;
    sqlOperation(sqlCode,&check);
    
    check.assign(check,0,username.size());
    if(check != username){
        num = 0;
    }else{
        sqlCode = "select max(id) from message where sendto ='"+username + "';";
        sqlOperationNum(sqlCode, &num);
    }
    
    if(num == 0){
        cout << endl << "You have (" << num <<") Message." << endl;
    }else{
        cout << "You have (" << num << ") messages." <<endl;
        int index;
        stringstream s;
        string fromwho;
        string data;
        for(index=1;index<=num;index++){
            s << index;
            cout << index << "| ";
            sqlCode = "select fromwho from message where id = " + s.str();
            sqlCode = sqlCode + " and sendto = '"+username+"';";
            sqlOperation(sqlCode, &fromwho);
            cout << "From: " << fromwho << "| ";
            
            sqlCode = "select date from message where id = " + s.str();
            sqlCode = sqlCode + " and sendto = '"+username+"';";
            sqlOperation(sqlCode, &data);
            cout << "Date: " << data << endl;
            
            s.str(string());
            fromwho = "";
            data = "";
        }
    }
    
    return num;
}

void setColumn(string username , string num, string content, string* ptr){
    
    string sqlCode;
    
    sqlCode = "select "+ content + " from message where sendto = '"+username+"' and id ="+num+";";
    sqlOperation(sqlCode,ptr);
    
    
}

int getMessage(string username , string num, string content, Message* ptr){
    
    string sqlCode;
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    sqlCode = "select "+ content + " from message where sendto = '"+username+"' and id ="+num+";";
    
    string sql = sqlCode; 
    
    rc = sqlite3_exec(db, sql.c_str(), callbackmesg, ptr, &zErrMsg);
    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }  
    sqlite3_close(db);
    return 0;
}

using std::vector;
using std::string;

void write_vec(const vector<string>& vec) {
    
    stringstream s;
    char chr;
    string byte;
    unsigned int len;
    string hexMessg;
    unsigned int index;
    string newString;
    for (vector<string>::const_iterator iter = vec.begin();iter != vec.end(); ++iter) {
            s << *iter;
            hexMessg = s.str();
    }
    len = hexMessg.length(); 
    for(index=0;index<len;index+=2){
        byte.assign(hexMessg.c_str(),index,2);
        chr = (char)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    cout << newString;

}

void readMessage(string username,unsigned int messageNum){
    
    ostringstream s;
    s << messageNum;
    string num;
    num = s.str();
    
    Message m;
    vector<string> message;
    
    //get message info
    setColumn(username,num,"fromwho",&(m.fromwho));
    setColumn(username,num,"date",&(m.date));
    setColumn(username,num,"title",&(m.title));
    
    getMessage(username,num,"messg",&m);
    
    cout << endl << "\nFROM: " << m.fromwho << "@geemail.jinqian" << endl;
    cout <<"DATE: " << m.date << endl;
    cout <<"TITLE: " << m.title << endl;
    write_vec(m.mesg);
    cout << "press any key + ENTER to continue ...\n\n" << endl;
    
}

bool sendMessage(string fromwho,string sendto,string passphrase){
    
    bool sendSuccess = false;
    int max_num;
    string sqlCode;
    
    sqlCode = "select sendto from message where sendto ='"+ sendto +"';";
    string check;
    sqlOperation(sqlCode,&check);
    
    check.assign(check,0,sendto.size());
    if(check != sendto){
        max_num = 0;
    }else{
        sqlCode = "select max(id) from message where sendto ='"+ sendto + "';";
        sqlOperationNum(sqlCode, &max_num);
    }
    max_num = max_num+1;
    stringstream s;
    s << max_num;
    string num;
    num = s.str();
    
    string salt;
    salt = userSaltGenertator();
    string passphraseHash;
    passphraseHash = generatePasswordHash(sendto,passphrase,salt);
    
    string message;
    string mesgLine;
    bool checkEnd;
    checkEnd = true;
    ostringstream hexIt;
    string hexMesg;
    unsigned int index;
    string title;
    bool invalidInput;
    
    invalidInput = true;
    while(invalidInput){
        try{
            cout << "Enter Title (max size: 40) "<< endl;
            cin.ignore();
            getline(cin,title);
            if(title.size()>40){
                throw inputSizeError();
            }
            invalidInput = false;
        }catch(inputSizeError e){
            cout << endl << e.what() << endl;
        }
    }
    
    cout << endl << "FROM: " << fromwho << "@geemail.jinqian" << endl;
    cout << "SEND TO: " << sendto << "@geemail.jinqian" << endl;
    cout << "Use 'enter+:q!+enter' to end writing.\n" << endl;
    
    string messOriginal;
    while(checkEnd){
        getline(cin,mesgLine);
        messOriginal.append("\n");
        if(mesgLine == ":q!"){
            checkEnd = false;
        }else{
            messOriginal.append(mesgLine);
            checkEnd = true;
            //message.append(hexMesg);
        }
    }
    for(index=0;index<messOriginal.length();index++){
        hexIt << hex << setfill('0') << setw(2) << (int)messOriginal[index];
    }
    hexMesg = hexIt.str();
    message.append(hexMesg);
    
    time_t rawtime; 
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    string date(buffer);
    
    Message messg;
    
    messg.num = num;
    messg.fromwho = fromwho;
    messg.sendto = sendto;
    messg.date = date;
    messg.title = title;
    messg.salt = salt;
    messg.passphrase = passphraseHash;
    
    sendSuccess = insertMessageColumn(&messg,message);
   
    return sendSuccess;
}

bool insertMessageColumn(Message* messg,string mesg){
                             
    string sqlCode;
    
    sqlCode = "insert into message (id,fromwho,sendto,date,title,passphrase,messg,salt) values (";
    sqlCode+= "'"+ messg->num + "',";
    sqlCode+= "'"+ messg->fromwho + "',";
    sqlCode+= "'"+ messg->sendto + "',";
    sqlCode+= "'"+ messg->date + "',";
    sqlCode+= "'"+ messg->title + "',";
    sqlCode+= "'"+ messg->passphrase + "',";
    sqlCode+= "'"+ mesg + "',";
    sqlCode+= "'"+ messg->salt + "');";
    
    sqlOperation(sqlCode, NULL);
    
    return true;
                                 
}