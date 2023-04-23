## 1

`http://10.15.111.100/game1/`

### Step 1

> 1. View page source.

​	Using F12 to open DevTool, we can find that:

- The comment says that `The password is 029c64152b6954e91d39183f8d2e07a9`
- When we click the button `提交`, function `check()` would check whether the value equals to `029c64152b6954e91d39183f8d2e07a9`. If so, the page will redirect to `l3vel2.html`

​	Thus, we can infer that: the password of this step is `029c64152b6954e91d39183f8d2e07a9`.

![1-1-1](/Users/shen/Desktop/网安实践/labs/Lab1/img/1-1-1.png)

​	After entering the password, we then go to the next step.

> Actually, we can visit `http://10.15.111.100/game1/l3vel2.html` directly.

### Step 2

> 2. View the source.

​	Since I use F12 to open the DevTool, blocking the right click doesn't bother me(?)

![1-2-1](/Users/shen/Desktop/网安实践/labs/Lab1/img/1-2-1.png)

​	Just like what I have done in step 1: entering `b910592a8ff0f56123105740c1735eb0`, or visit `http://10.15.111.100/game1/Y0uR666.php` directly.

### Step 3

> 3. Capture RESPONSE-packet header using Burp Suite.

​	Since there is no suitable version of Burp Suite for my PC(Mac with Apple Silicon), I tried to user DevTool to fix this problem(the jar file somehow  can't work properly).

<img src="/Users/shen/Desktop/网安实践/labs/Lab1/img/sth.png" alt="sth" style="zoom:30%;" />

​	It's not hard to find the FLAG at the header of the RESPONSE-packet, which is `ACTF{2650e41ce3e251bfd29527b5dff707ee}`

![1-3-2](/Users/shen/Desktop/网安实践/labs/Lab1/img/1-3-2.png)

<center><b>Thus, the FLAG is ACTF{2650e41ce3e251bfd29527b5dff707ee}</b></center>

## 2

`http://10.15.111.100/game2`

### Step 1

> 1. View page source.
> 1. Understand 302 redirection.

​	The comment told us to pay attention to the 302 redirection, which ndicates that the resource requested has been temporarily moved to the URL given by the `Location` header. 

<img src="/Users/shen/Desktop/网安实践/labs/Lab1/img/do-u-know.png" alt="do-u-know" style="zoom:50%;" />

> 3. Locate redirected pages and find password.

​	By using Burp Suite, we can find the response with status 302, which told us that the password is `80e20d8fe7edfbeb591750ba31a59d07`.

![302](/Users/shen/Desktop/网安实践/labs/Lab1/img/302.png)

​	Enter the password, and we can move to the next step.

### Step 2

> 4. Understand HTTP Referer field.

​	The `Referer` HTTP request header contains the absolute or partial address from which a resource has been requested. It can prevent stealing link and blocking malicious requests.

> 5. Capture GET-packet and rewrite Referer filed using Burp Suite.

![local-only](/Users/shen/Desktop/网安实践/labs/Lab1/img/local-only.png)

​	Since the website denied our access because of we are not from `http://localhost/` we can use the Repeater of Burp Suite to rewrite Referer field accordingly:

![add-referer](/Users/shen/Desktop/网安实践/labs/Lab1/img/add-referer.png)

​	Then, we can get the password in the RESPONSE-packet:

![get-pwd](/Users/shen/Desktop/网安实践/labs/Lab1/img/get-pwd.png)

### Step 3

> 6. Capture GET-packet and rewrite Cookie field with admin privilege using Burp Suite.

​	Since the website said that 'only the admin can see the Flag', and the Cookie field fo the GET-packet equals to `admin=0`. We can guess that `admin=1` means the request is from the admin.

![need-cookie](/Users/shen/Desktop/网安实践/labs/Lab1/img/need-cookie.png)

​	We can rewrite the packet accordingly then:

![rewrite-cookie](/Users/shen/Desktop/网安实践/labs/Lab1/img/rewrite-cookie.png)

​	The RESPONSE-packet show the Flag this time:

![get-flag](/Users/shen/Desktop/网安实践/labs/Lab1/img/get-flag.png)

<center><b>Thus, the FLAG is ACTF{47ca8aa874ba92a4362ld5ffBedededfl}</b></center>

## 3



`http://10.214.160.13:10000`

### Step 1

> 1. View page source.

​	Using F12 to open DevTool and then unfold `<body>` tag, we can discover that `删除1.php.bak` has been commented.

![3-1-1](/Users/shen/Desktop/网安实践/labs/Lab1/img/3-1-1.png)

​	Thus, I try to visit `http://10.214.160.13:10000/1.php.bat`, the content of `1.php.bat` is as follows:

> 2. Get link from `.bak` file.

<img src="/Users/shen/Desktop/网安实践/labs/Lab1/img/3-1-2.png" alt="3-1-2" style="zoom:50%;" />

​	It seems that the `<a>` tag aiming to redirect us to `http://10.214.160.13:10000/the2nd.php`

### Step 2

> 3. Capture GET-packet and null Referer filed using Burp Suite.

​	In this step, my Firefox browser redirect me to the next step directly, while Chrome show me the alert '你从哪里来？'. Contrasting the difference of two GET-packet, we can guess that the null-referee field might be the cause.

![compare](/Users/shen/Desktop/网安实践/labs/Lab1/img/compare.png)

​	Add correct Referee field to the packet, it leads us to the next step.

### Step 3

> 4. Capture RESONSE-packet header with next link included using Burp Suite.

​	Click the button in `3rd.php`, it direct us to `http://10.214.160.13:10000/di4guan.php` then.

​	According to the hint, we can view the RESPONSE-packet of `di4guan.php` :

![get-next](/Users/shen/Desktop/网安实践/labs/Lab1/img/get-next.png)

​	The Next field in the header seems to be the URL of the next step.

​	Visit `http://10.214.160.13:10000/wozaizheli.php` directly, we can then go to the next step.	

### Step 4

> 5. View page source and try to click the button or craft packet with button click effect.

​	Function `joy()` add style `display:none` to `#joy` when the mouse is over this element.

![find-prob](/Users/shen/Desktop/网安实践/labs/Lab1/img/find-prob.png)

​	Since modifying the JS in DevTool wouldn't work this time, I try to trigger the click event in the console this time:

1. Get the submit button by its Id.
2. Trigger the click event of the button.

![by-console](/Users/shen/Desktop/网安实践/labs/Lab1/img/by-console.png)

​	We can finally get the Flag from the website:

![fin](/Users/shen/Desktop/网安实践/labs/Lab1/img/fin.png)

<center><b>Thus, the FLAG is AAA(yOu 2a gOod front-end Web developer}</b></center>