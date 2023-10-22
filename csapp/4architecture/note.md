# 4 处理器体系结构

## 笔记

## 习题

4.13
![题目4.13](image.png)
图4-17:
![图4-17](image-1.png)
irmovq是一条传送指令，将立即数存到寄存器里。具体的编码参考：
![Y86-64指令集](image-2.png)
Y86-64所有指令
答案：  
|阶段|操作|
|---|---|
|取指|icode:ifun $\leftarrow$ $M_1$[PC] = $M_1$[0x016] = 3:0 第一个字节存操作码<br>rA:rB $\leftarrow$ $M_1$[PC+1] = $M_1$[0x017] = f:4 第二个字节存寄存器编号<br>valC $\leftarrow$ $M_8$[PC+2] = $M_8$[0x018] = 00000000 00000080 = 8 $\times$ 16 = 128 高八字节存立即数，因为是小端存储，所以80是在低位<br>valP $\leftarrow$ PC + 10 = 0x20 这条指令是10字节长，PC+10|
|译码|第一个操作数是立即数，不需要取第二个寄存器操作数，所以该指令在译码阶段什么都不做|
|执行|valE $\leftarrow$ 0 + valC = 128|
|访存||
|写回|R[rB] $\leftarrow$ valE = 128|
|更新PC|PC $\leftarrow$ valP = 0x20|
