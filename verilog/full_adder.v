module full_adder (
    input A, B, Cin, // 输入信号 A, B 和进位输入 Cin
    output Sum, Cout // 输出和 Sum 和进位输出 Cout
);

    // 使用异或门计算和
    assign Sum = A ^ B ^ Cin;

    // 使用与门计算进位
    assign Cout = (A & B) | (A & Cin) | (B & Cin);

endmodule