 # Homework of Chap. 2 - 3

> 姓名：沈韵沨
>
> 学号：3200104392

## Chapter 2

1. **If a binary signal is sent over a 3-kHz channel whose signal-to-noise ratio is 20 dB, what is the maximum achievable data rate?**

​			According to the *Shannon Theory*：$$\max(data\_rate) = B\log_2{(1+\frac{S}{N})}$$

​			$$\because signal\_to\_noise\ ratio = 10 \log_{10}{\frac{S}{N}} = 20dB$$

​			$$\therefore \frac{S}{N} = 100$$

​          $$ \Longrightarrow \max(data\_rate) = B \log_2{1 + 101} = 3k\ *\log_2{101} = 19.98kbps$$

​			So, the maximum achievable data rate is <u>19.98kbps</u>.

2. **What signal-to-noise ratio is needed to put a T1 carrier on a 50-kHz line?**

   According to the *Shannon Theory*：$$\max(data\_rate) = B\log_2{(1+\frac{S}{N})}$$

​			$$\because T_1\ carrier's\ bandwidth = 1.544\ *\ 10^6 = 50k\ *\ \log_2{(1+\frac{S}{N})}$$

​			$$\therefore \frac{S}{N} = 2^{\frac{1544}{50}}-1 \approx 2^{31} -1 = 2147483647  $$

​			So, to put a T1 carrier on a 50-kHz line, the signal_to_noise ratio need to be about <u>2147483647</u>.

3. **Ten signals, each requiring 4000 Hz, are multiplexed on to a signal channel using FDM. How much minimum bandwidth is required for the multiplexed channel? Assume that the guard bands are 400 Hz wide.**

​			If 10 signal channel are multiplexed, then 9 guard bands are needed.
​         $$\Longrightarrow 4000*10\ +\ 400*9 = 43600$$

​			Thus, the minimum bandwidth required for the multiplexed channel is <u>43600Hz</u>.

## Chapter 3

1. **A bit string, 0111101111101111110, needs to be transmitted at the data link layer. What is the string actually transmitted after bit stuffing?**

   ```
   Original: 01111011111   011111   10
   Process:  01111011111 0 011111 0 10
   Result:   011110111110011111010
   ```

​			The string actually being transmitted is 011110111110011111010.

2. **What is the remainder obtained by dividing x^7 + x^5+1 by the generator polynomial x^3 +1 ? (注：x^7表示x的7次方，其它表述方式相同)** 

   ```
                    x^4 + x^2 + x
   x^3 + 1 /  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅  ̅ ̅ ̅ 
          /   x^7 + x^5 + 1
              x^7 + x^4
           -------------------
                    x^5 + x^4 + 1
                    x^5 + x^2 + 1
                --------------------
                          x^4 + x^2 + 1
                          x^4 + x
                      ---------------------
                               x^2 + x + 1
   ```

   Thus, the remainder is <u>x^2 + x +1</u>.

3. **Data link protocols almost always put the CRC in a trailer rather than in a header. Why?**

   ​        First, CRC is computed during transmission, and then appended to the output stream, as the last -bit goes out on the wire. 

   ​        If CRC were in the header, we should pass the frame in order to compute CRC BEFORE transmitting. In this way, each byte would be handled **TWICE**(checksumming & transmitting). 

   ​        While, if we put CRC in a trailer, each byte would be handled only **ONCE**.

   ​        That is why data link protocols almost always put the CRC in a trailer, rather than in a header.

4. **Frames of 1000 bits are sent over a 1-Mbps channel using a geostationary satellite whose propagation time from the earth is 270 msec. Acknowledgements are always piggybacked onto data frames. The headers are very short. Three-bit sequence numbers are used. What is the maximum achievable channel utilization for** 

​			**a）Stop-and-wait**

​			**b）Protocol 5**

​			**c）Protocol 6**

​			Assuming the transmission starts at t = 0.

​			For the frame-size = 1000b, and  the transmission rate is 1Mbps:
​			the 1-st frame would be fully transmitted at t = 1000/1M = 1 ms.

​			Since the propagation time from earth is 270 ms:
​			the 1-st frame would fully arrived at t = 270+1 = 271 ms.

​			Likewise, at t = 271+1 = 272 ms, the frame acknowledging the 1-st one would been fully sent.
​			And at t = 272+270 = 542 ms, the acknowledging frame would fully arrived.

​			Thus, the cycle = 542 ms.

​			Assuming that we send n*frames during 542ms:
​			Since the transmission of each frame would take 1ms, the efficiency = n/cycle = n/542.

​			a）$$k = 1 \Longrightarrow e = \frac{1}{542} = 0.18\%$$

​			b）$$k = 7 \Longrightarrow e = \frac{7}{542} = 1.29\%$$

​			c）$$k = 4 \Longrightarrow e = \frac{4}{542} = 0.74\%$$

5. **What is the minimum overhead to send an IP packet using PPP? Count only the overhead introduced by PPP itself, not the IP header overhead.**

​			Minimum overhead: 5 Byte.

​			For each frame: 2 * flag + 1 * protocol + 2 * checksum

 