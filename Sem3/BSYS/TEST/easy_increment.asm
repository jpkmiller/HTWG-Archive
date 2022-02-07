
easy_increment:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <func>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
   8:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
   c:	90                   	nop
   d:	5d                   	pop    %rbp
   e:	c3                   	retq   
