section .text  
extern main  
global start  
  
start:  
    cli                    ; 关闭中断  
    mov ax, 0x10            ; 把0x10送入ax，0x10是BIOS中断向量表中的屏幕上打印一个字符的程序入口点  
    int 0x15                ; 调用BIOS中断，把屏幕上打印一个字符的程序加载到内存中  
    call main               ; 调用main函数  
    hlt                     ; 挂起CPU，等待下一个中断