## Environment

- Ubuntu 18.04 in VMware

- ARP Spoofing Attack Tool: `dsniff`
- DNS Spoofing Attack Tool: `ettercap` 

## Set Up

|          Machine          | IP address  |    MAC address    |
| :-----------------------: | :---------: | :---------------: |
|       Host(Victim)        | 172.20.10.3 | B8-9A-2A-2C-99-08 |
| Virtual Machine(Attacker) | 172.20.10.8 | 00-0C-29-C8-BD-80 |
|          Gateway          | 172.20.10.1 | 8A-A4-79-E3-18-64 |

![ips](/Users/shen/Desktop/网安实践/labs/Lab2/pics/ips.png)

<center>IP information of the Host & Virtual Machine</center>

![pings](/Users/shen/Desktop/网安实践/labs/Lab2/pics/pings.png)

<center>Virtual Machine & the Host can ping each other</center>

​	Using Wireshark to capture packet, we can see that the Virtual Machine continuously tell that 172.20.10.1(IP addres of the Gateway) is at 00-0C-29-C8-BD-80(MAC address of the Virtual Machine):

![ARP](/Users/shen/Desktop/网安实践/labs/Lab2/pics/ARP.png)

## ARP Spoofing

### MAC sniffing

![原始arp](/Users/shen/Desktop/网安实践/labs/Lab2/pics/原始arp.png)

<center>Original MAC address of Gateway & Virtual Machine</center>

![攻击者arp](/Users/shen/Desktop/网安实践/labs/Lab2/pics/攻击者arp.png)

<center>Original MAC address of the Host</center>

### ARP Spoofing Attack

​	We use `disniff` to apply ARP Spoofing Attack, the instruction is as follows:

​	`sudo arpspoof -i ens33 -t 172.20.10.3 172.20.10.1`

![arp-fin](/Users/shen/Desktop/网安实践/labs/Lab2/pics/arp-fin.png)

​	We can find that in the ARP cache of the Host, the Gateway's MAC address is the same as the Virtual Machine's.

## DNS Spoofing

![原始ping通](/Users/shen/Desktop/网安实践/labs/Lab2/pics/原始ping通.png)

<center>The Host can ping target website correctly before attack</center>

### DNS Spoofing Attack

​	We use `ettercap GUI` to apply DNS Spoofing Attack:

1. Edit `etter.dns`.
   Map www.baidu.com & www.bilibili.com to `8.136.83.180`
   ![modify](/Users/shen/Desktop/网安实践/labs/Lab2/pics/modify.png)

2. Use `ettercap GUI` to do unified sniping on `ens33`

3. Scan hosts and add the Host(Victim) & the Gateway as targets
   ![targets](/Users/shen/Desktop/网安实践/labs/Lab2/pics/targets.png)

4. Use plugin `dns-spoofing` to start attack, at the same time, do ARP poisoning.

5. Enter `ipconfig /flushdns` and then ping target websites in the Host.

   ![poisoned](/Users/shen/Desktop/网安实践/labs/Lab2/pics/poisoned.png)
   <center>Both of them were spoofed to the wrong IP address</center>

6. Observe the sniffing result in the Virtual Machine(Attacker), we can view the same result:
   ![spoof](/Users/shen/Desktop/网安实践/labs/Lab2/pics/spoof.png)

​	Using Wireshark to capture the packages. 

​	In DNS packages, it seems that "172.20.10.1(the Gateway)" told the victim that all those targets are at 8.136.83.180:

![DNS](/Users/shen/Desktop/网安实践/labs/Lab2/pics/DNS.png)

​	But we use `ettercap` to do ARP poisoning at the same time, viewing the ARP packages, we can see that: the Virtual Machine also cheat on the MAC address of the Gateway.

![DNS-PLUS](/Users/shen/Desktop/网安实践/labs/Lab2/pics/DNS-PLUS.png)

​	Thus, in the DNS Spoofing Attack, `ettercap` first disguised as the Gateway by response the its own MAC address, then offer the spoofed DNS RESPONSE to the victim as its 'Gateway'.