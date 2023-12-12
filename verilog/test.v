`timescale 1ns/1ns

module test;

  // 定义被测试的半加器模块实例
  half_adder dut(
    .A(A),
    .B(B),
    .Sum(Sum),
    .Carry(Carry)
  );

  // 输入信号和输出信号的声明
  reg A;
  reg B;
  wire Sum;
  wire Carry;

  // 初始化输入信号
  initial begin
    A = 0;
    B = 0;
    
    // 执行测试用例 1: A=0, B=0
    #10;
    A = 0;
    B = 1;
    
    // 执行测试用例 2: A=0, B=1
    #10;
    A = 1;
    B = 0;
    
    // 执行测试用例 3: A=1, B=0
    #10;
    A = 1;
    B = 1;
    
    // 执行测试用例 4: A=1, B=1
    
    #10;
    $finish;
  end

  // 监控输出信号
  always @(posedge Carry or posedge Sum) begin
    $display("A=%b, B=%b, Sum=%b, Carry=%b", A, B, Sum, Carry);
  end

endmodule