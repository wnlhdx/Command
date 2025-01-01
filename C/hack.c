#include <stdio.h>
#include <string.h>

void secret() {
    printf("You have been hacked!\n");
}

void vulnerable() {
    char buffer[64];
    printf("Buffer address: %p\n", buffer); 
    gets(buffer);  // 不进行边界检查，易受缓冲区溢出攻击
}

int main() {
    vulnerable();
    return 0;
}
