// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

// i = R1
@R1
D=M
@i
M=D

// result = 0
@result
M=0

(LOOP)
@i
D=M
@STOP
D;JEQ   // if i == 0 then goto STOP

@i
M=M-1   //i-=1

@R0
D=M
@result
M=M+D   //result += R0

@LOOP
0;JMP   //if i > 0 then goto LOOP

(STOP)
// R2=result
@result
D=M
@R2
M=D

(END)
@END
0;JMP