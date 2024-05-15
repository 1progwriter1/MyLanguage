global my_scanf

                STDIN      equ 0

section .bss
                buffer  resb 16
                tmp_buf resb 16

 section .text

 ;----------------------------------------------
 ;my_scanf - my version of C scanf
 ;Expects: none
 ;Returns: rax - number
 ;Destorys: none
 ;----------------------------------------------

 my_scanf:
                push rdx                        ;save
                push rdi
                push rsi

                mov rax, 0x00                   ;write
                mov rdi, STDIN                  ;fd
                mov rdx, 16                     ;buffer length
                mov rsi, buffer
                syscall

                call convert                    ;convert to number

                pop rsi
                pop rdi                         ;restore
                pop rdx
                ret

;----------------------------------------------
;Converts string to decimal number
;Expects: rsi - string
;Returns: rax - number
;Destorys: rsi
;----------------------------------------------
convert:
                push rcx                        ;save
                push rdx
                push r8
                xor rcx, rcx
                mov dl, [rsi]
                cmp dl, '-'
                je .set_minus
                mov r8, 1
.back:

.find_end:
                mov dl, [rsi]                   ;current symbol
                cmp dl, 0xA                     ;check end
                je .found
                inc rcx                         ;increase length
                inc rsi                         ;next symbol
                jmp .find_end
.found:
                xor rax, rax
                cmp rcx, 0
                je .end                         ;check if empty string
                dec rsi                         ;prev symbol
                xor rdx, rdx
.loop:
                xor rdx, rdx
                mov dl, [rsi]                   ;current symbol
                sub dl, '0'                     ;find out number
                imul rdx, r8                    ;rdx *= base
                imul r8, 10                     ;base *= 10
                add rax, rdx                    ;add to result
                dec rsi                         ;next symbol
                loop .loop
.end:
                pop r8
                pop rdx
                pop rcx

                ret

.set_minus:
                mov r8, -1
                inc rsi
                jmp .back