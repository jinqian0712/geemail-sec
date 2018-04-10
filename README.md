GeeMail (C/C++)

What is your project?
geemail
https://ide.c9.io/jinqian/geemail

How do I run the program?
complier with ‘g++ *.cpp -o geemail -lsqlite3 -lgcrypte’ then, run it with ‘./geemail’ .
using the ‘R’ or ‘r’ for sign up an account and the ’S’ or ’s’ for login. /*when I testing the program, I already created a few account at the
‘test.db’. */
the ‘testuser’, ‘testuser2’, ‘jinqian’ is account created before the
   
password hash generator code and salt generator code was written. So is impossible for you to login to those account. But you can test the code by login the following account. And also you can create your own account by enter ‘R’ or ‘r’.
// username: leishiting password: 111111
// username: jinqian0712 password: 111111
// username: testtest password: 111111
to read the message I created before, all the message passphrase is ‘123456’.
After you sign in the user account, it will show you the login page. Then, you can enter ‘R or r’ for read the message and ’S or s’ for send message.
 if you enter ’s’, the code will bring you to the read mod.After that, you can select witch message you want to read. Then, you can enter the message passphrase to read the message.
if you choose the ’S/s’ for send message. you can follow the program enter your title and message content. When you write your message you can enter ‘:q!’ at a new line anytime to finish the message input.

if you choose the ’S/s’ for send message. you can follow the program enter your title and message content.
 
To read the message I just send. You can quit the program and run it again. Sign in the account ‘testtest’ to read the message.