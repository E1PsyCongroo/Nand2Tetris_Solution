// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

// width of screen: 32
@32
D=A
@width
M=D

// height of screen: 256
@256
D=A
@height
M=D

(CLEAN)
// for i in range(16):
//      for j in range(32):
//          RAM[screen + 32*i + j] = 0

// addr = SCREEN
@SCREEN
D=A
@addr
M=D

// i=0
@i
M=0

// clean screen
(LOOP4)
// j = 0
@j
M=0
(LOOP5)
@addr   // RAM[addr] = 0; addr+=1
A=M
M=0
@addr
M=M+1

@j      // j++
M=M+1
D=M
@width
D=D-M
@LOOP5  // if j-width != 0 then goto LOOP5
D;JNE

@i      // i++
M=M+1
D=M
@height
D=D-M
@LOOP4  // if i-height != 0 then goto LOOP4
D;JNE

(END2)
@KBD
D=M
@END2
D;JEQ

// for i in range(16):
//      for j in range(32):
//          RAM[screen + 32*i + j] = -1

// addr = SCREEN
@SCREEN
D=A
@addr
M=D

// i=0
@i
M=0

// draw screen
(LOOP2)
// j = 0
@j
M=0
(LOOP3)
@addr   // RAM[addr] = -1; addr+=1
A=M
M=-1
@addr
M=M+1

@j      // j++
M=M+1
D=M
@width
D=D-M
@LOOP3  // if j-width != 0 then goto LOOP3
D;JNE

@i      // i++
M=M+1
D=M
@height
D=D-M
@LOOP2  // if i-height != 0 then goto LOOP2
D;JNE

(END1)
@KBD
D=M
@CLEAN
D;JEQ
@END1
0;JMP