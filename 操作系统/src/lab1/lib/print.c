// lib/print.c
#include "print.h"
#include "sbi.h"

void puts(char *s) {
    char idx = 0;
    while(s[idx] != '\0') {
        sbi_ecall(0x1, 0x0, (int)s[idx], 0, 0, 0, 0, 0); // 调用4.4中完成的sbi_ecall函数打印单个字符
        idx++;
    }
}

void puti(int x) {
    if(x) {
        char tmp[11] = "\0"; // 用于存储倒序数字串
        int len = 0;
        while(x) {
            tmp[len++] = x%10 + 0x30;
            x /= 10;
        }

        int i = 0;
        while(i < len) { // 正序输出数字串
            sbi_ecall(0x1, 0x0, (int)tmp[len-i-1], 0, 0, 0, 0, 0);
            i++;
        }
    }
    else { //单独处理x==0 的情况
        sbi_ecall(0x1, 0x0, 0x30, 0, 0, 0, 0, 0);
    }
}
