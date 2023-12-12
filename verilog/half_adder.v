module half_adder(
    input A, // 输入信号 A
    input B, // 输入信号 B
    output Sum, // 输出和
    output Carry // 输出进位
);

assign Sum = A ^ B; // 通过异或门计算和
assign Carry = A & B; // 通过与门计算进位

endmodule