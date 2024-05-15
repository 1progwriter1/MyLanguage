global my_printf

                 Buffer_Len  equ 64
                 STDOUT      equ 1

 section .bss
                 buffer resb Buffer_Len       ;create buffer

 section .text

 ;----------------------------------------------
 ;my_printf - my version of C printf
 ;Expects:   rdi - const char *format str - format string, ended with \0
 ;           rsi, rdx, rcx, r8, r9, stack ... - arguments
 ;Returns: none
 ;Destorys: rdi
 ;----------------------------------------------
 ;Using registers for:
 ;       rdi - current symbol in fmt string
 ;       rsi - string to print (argument saved to r13)
 ;       rdx - length (argument saved to the r12)
 ;       rax - current argument
 ;       rbp - pointer to the arguments in stack
 ;       rbx - index of the current argument
 ;       rcx - saved to r14
 ;----------------------------------------------


 my_printf:
                 push rbp                        ;save rbp
                 mov rbp, rsp
                 add rbp, 16                     ;set rbp to the 7th argument

                 push r12                        ;save registers
                 push r13
                 push r14
                 push rcx
                 push rsi
                 push rdx
                 push rax
                 push rbx

                 mov r12, rsi                    ;save arguments to use registers
                 mov r13, rdx
                 mov r14, rcx

                 mov rsi, rdi                    ;save string to rsi
                 xor rdx, rdx                    ;start length = 0

                 xor rbx, rbx                    ;index of the current argument(except fmt string)

 .format_str_loop:
                 cmp byte [rdi], 0x0             ;check if end
                 je .end_format_string
                 inc rdx                         ;length++

                 cmp byte [rdi], '%'
                 jne .continue
                 call PercentProcess             ;processing the % (rdi = 0 after this call, rsi - next symbol)
                 jmp .format_str_loop
 .continue:
                 inc rdi                          ;next symbol
                 jmp .format_str_loop


 .end_format_string:

                 call Print                      ;printing the end of the fmt string

                 pop rbx                         ;recover registers
                 pop rax
                 pop rdx
                 pop rsi
                 pop rcx
                 pop r14
                 pop r13
                 pop r12
                 pop rbp

                 ret


 ;---------------------------------------------

 ;---------------------------------------------
 ;Finds out % argument and prints necessary value
 ;Expects: rdi - percent pointer
 ;Returns: rdx = 0, rsi - address of the next symbol
 ;Destorys: rax
 ;---------------------------------------------
 PercentProcess:
                 dec rdx                         ;decrease rdx not to print %
                 call Print                      ;print text before %

                 inc rdi                         ;rdi++ - now set to the % argument
                 mov rsi, buffer                 ;buffer is used to create output

                 cmp byte [rdi], '%'
                 je .percent                     ;print percent

                 call GetArgument                ;write argument to rax
                 call PrintArgument              ;print argument
 .end_process:
                 inc rdi                         ;next symbol
                 xor rdx, rdx                    ;length = 0
                 mov rsi, rdi                    ;rsi - skip %_
                 ret

 .percent:
                 mov byte [buffer], '%'
                 mov rsi, buffer                 ;rsi <- %
                 mov rdx, 1                      ;length = 1
                 call Print                      ;print percent
                 jmp .end_process

 ;---------------------------------------------
 ;Gets argument for %
 ;Expects: rbx - index of current argument, rbp - 7th argument from stack
 ;Returns: rax - argument, rbx - index of the next argument
 ;Destorys: none
 ;---------------------------------------------
 GetArgument:
                 cmp rbx, 5                      ;5 - number of arguments not in the stack
                 jae .stack

                 jmp [JmpTableArg + rbx * 8]

 .end_argument:
                 inc rbx                         ;rbx++ - next argument
                 ret
 .Arg1:
                 mov rax, r12                    ;rax <- r12
                 jmp .end_argument
 .Arg2:
                 mov rax, r13                    ;rax <- r13
                 jmp .end_argument
 .Arg3:
                 mov rax, r14                    ;rax <- r14
                 jmp .end_argument
 .Arg4:
                 mov rax, r8                     ;rax <- r8
                 jmp .end_argument
 .Arg5:
                 mov rax, r9                     ;rax <- r9
                 jmp .end_argument

 .stack:
                 push rbx                        ;save rbx
                 sub rbx, 5                      ;rbx -= 5 - to create correct offset in stack
                 mov rax, [rbp + rbx * 8]        ;rax <- rbp + 8 * rbx
                 pop rbx                         ;recover rbx
                 inc rbx                         ;rbx++
                 ret

 ;---------------------------------------------
 ;Prints argument in necessary format
 ;Expects: rdi - format, rax - argument, rsi - buffer
 ;Returns: none
 ;Destorys: rcx
 ;---------------------------------------------
 PrintArgument:
                 xor rcx, rcx                    ;rcx <- 0
                 mov cl, [rdi]                   ;rcx <- argument type
                 cmp cl, 'b'
                 jb .error
                 cmp cl, 'x'
                 ja .error
                 sub cl, 'b'                     ;cl -= 'b'
                 jmp [JmpTableType + rcx * 8]

 .print_buffer:
                 call Print                      ;print created buffer
                 ret

 .character:
                 mov [buffer], rax               ;buffer <- symbol
                 mov rdx, 1                      ;length = 1
                 jmp .print_buffer
 .string:
                 mov rsi, rax                    ;rsi <- string from rax
                 call Strlen                     ;find length of the string
                 jmp .print_buffer

 .decimal:
                 call DecimalNumberToText        ;write number to the buffer as a text
                 jmp .print_buffer
 .octal:
                 mov cl, 3                       ;shift for octal number
                 mov rdx, 0x7                    ;mask for the three lower bits
                 jmp .L1
 .hexadecimal:
                 mov cl, 4                       ;shift fot hexadecimal number
                 mov rdx, 0xf                    ;mask for the four lower bits
                 jmp .L1
 .binary:
                 mov cl, 1                       ;shift for binary number
                 mov rdx, 0x1                    ;mask for the lower bit
                 jmp .L1

.error:
                mov byte [buffer], 21                ;buffer <- !
                mov rdx, 1                      ;length = 1
                jmp .print_buffer

 .L1:
                 call NumberToText
                 jmp .print_buffer

 ;---------------------------------------------
 ;Converts number to the text and writes it to the buffer
 ;Expects: rax - number, rdx - mask, cl - shift
 ;Returns: rdx - length of the number
 ;Destorys: rax, rcx, rdx
 ;---------------------------------------------
 NumberToText:
                 push r15                        ;save registers
                 push rbx
                 xor rbx, rbx                    ;length = 0
 .number_loop:
                 cmp rbx, Buffer_Len             ;loop_buffer if it was overflowed
                 jae .loop_buffer
 .L2:
                 mov r15, rax                    ;r15 <- rax
                 and r15, rdx
                 push r15                        ;stack <- r15
                 inc rbx                         ;length++
                 shr rax, cl                     ;rax >> shift
                 cmp rbx, Buffer_Len             ;overflow check
                 jae .end_number_loop
                 cmp rax, 0
                 ja .number_loop
 .end_number_loop:

                 mov rcx, rbx                    ;rcx <- rbx
                 xor rbx, rbx                    ;rbx <- 0

 .write_in_buffer:
                 pop r15                         ;rdx <- stack
                 mov [buffer + rbx], r15         ;write in buffer
                 cmp r15, 10                     ;check if number
                 jae .letter
                 add byte [buffer + rbx], '0'
 .L1:
                 inc rbx                         ;rbx++
                 cmp rbx, rcx                    ;check if end
                 jb .write_in_buffer

                 mov rdx, rbx                    ;rdx = length
                 pop rbx
                 pop r15
                 ret
 .letter:
                 add byte [buffer + rbx], 'a' - 10
                 jmp .L1

 .loop_buffer:
                 sub rsp, rbx
                 xor rbx, rbx
                 jmp .L2

 ;---------------------------------------------
 ;Converts decimal number to the text and writes it to the buffer
 ;Expects: rax - number
 ;Returns: rdx - length of the number
 ;Destorys: rax, rcx
 ;---------------------------------------------
 DecimalNumberToText:
                 call CheckIfNegative            ;print minus if necessary and rax = abs(rax)
                 push rbx
                 xor rbx, rbx                    ;length = 0
                 mov rcx, 10                     ;rcx <- base
 .number_loop:
                 cmp rbx, Buffer_Len             ;loop_buffer if it was overflowed
                 jae .loop_buffer
 .L1:
                 xor rdx, rdx                    ;rdx <- 0
                 div ecx                         ;edx:eax / ecx
                 push rdx                        ;stack <- rdx
                 inc rbx                         ;length++
                 cmp eax, 0
                 ja .number_loop

                 mov rcx, rbx                    ;rcx <- rbx
                 xor rbx, rbx                    ;rbx <- 0

 .write_in_buffer:
                 pop rdx                         ;rdx <- stack
                 mov [buffer + rbx], rdx         ;write in buffer
                 add byte [buffer + rbx], '0'    ;number -> text
                 inc rbx                         ;rbx++
                 cmp rbx, rcx                    ;check if end
                 jb .write_in_buffer

                 mov rdx, rbx                    ;rdx = length

                 pop rbx
                 ret

 .loop_buffer:
                 sub rsp, rbx
                 xor rbx, rbx
                 jmp .L1

 ;---------------------------------------------
 ;Finds length of the string ended with \0
 ;Expects: rsi - string
 ;Returns: rdx - length
 ;Destorys: none
 ;---------------------------------------------
 Strlen:
                 push rax                        ;save registers
                 push rdi
                 mov rdi, rsi                    ;rdi <- string
                 xor rdx, rdx                    ;current length = 0
                 mov al, 0x0                     ;al <- \0
 .length_loop:
                 scasb                           ;check if \0
                 je  .end_string
                 inc rdx                         ;length++
                 jmp .length_loop
 .end_string:
                 pop rdi                         ;recover registers
                 pop rax
                 ret

 ;--------------------------------------------
 ;Prints string
 ;Expects: rsi - string, rdx - length
 ;Returns: none
 ;Destorys: none
 ;--------------------------------------------
 Print:
                 push rax                        ;save registers
                 push rdi
                 mov rax, 0x01                   ;write
                 mov rdi, STDOUT
                 syscall
                 pop rdi                         ;recover registers
                 pop rax
                 ret

 ;-------------------------------------------

 ;-------------------------------------------
 ;Prints minus if necessary
 ;Expects: rax - number, rsi - buffer
 ;Returns: abs(rax)
 ;Destorys: none
 ;-------------------------------------------
 CheckIfNegative:
                 push rax                        ;save rax
                 and rax, 0x10000000             ;zero everything except sign byte
                 cmp rax, 0                      ;check if it is zero
                 jne .print_minus
                 pop rax                         ;recover rax
 .L1:
                 ret

 .print_minus:
                 pop rax                         ;recover rax
                 neg rax                         ;change sign
                 push rdx                        ;save rdx and rsi
                 push rsi
                 mov byte [buffer], '-'          ;write minus to the buffer
                 mov rdx, 1                      ;length = 1
                 call Print                      ;print minus
                 pop rsi                         ;recover rsi and rdx
                 pop rdx
                 jmp .L1

 ;------------------------------------------

 section .rodata

                 align 8
 JmpTableArg:
         dq      GetArgument.Arg1
         dq      GetArgument.Arg2
         dq      GetArgument.Arg3
         dq      GetArgument.Arg4
         dq      GetArgument.Arg5

 JmpTableType:
         dq      PrintArgument.binary            ;%b
         dq      PrintArgument.character         ;%c
         dq      PrintArgument.decimal           ;%d
         dq      PrintArgument.error
         dq      PrintArgument.octal             ;%o
         dq      PrintArgument.error
         dq      PrintArgument.string            ;%s
         dq      PrintArgument.error
         dq      PrintArgument.hexadecimal       ;%x