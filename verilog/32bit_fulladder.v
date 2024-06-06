module thirtytwo_bit_adder (
    input [31:0] A, // 32位加数 A
    input [31:0] B, // 32位加数 B
    input       Cin, // 进位输入
    output [31:0] Sum, // 32位和
    output      Cout // 进位输出
);

    // 使用全加器构建32位加法器
    wire [31:0] bit0_sum, bit0_carry;
    wire [31:0] bit1_sum, bit1_carry;
    wire [31:0] bit2_sum, bit2_carry;
    wire [31:0] bit3_sum, bit3_carry;

    // 定义全加器模块
    full_adder fa0 (.A(A[31:0]), .B(B[31:0]), .Cin(Cin), .Sum(bit0_sum), .Cout(bit0_carry));
    full_adder fa1 (.A(A[31:1]), .B(B[31:1]), .Cin(bit0_carry[31]), .Sum(bit1_sum), .Cout(bit1_carry));
    full_adder fa2 (.A(A[31:2]), .B(B[31:2]), .Cin(bit1_carry[31]), .Sum(bit2_sum), .Cout(bit2_carry));
    full_adder fa3 (.A(A[31:3]), .B(B[31:3]), .Cin(bit2_carry[31]), .Sum(bit3_sum), .Cout(bit3_carry));

    // 输出计算
    assign Sum = {bit3_sum, bit2_sum, bit1_sum, bit0_sum};
    assign Cout = bit3_carry;

endmodule