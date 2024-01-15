.model flat, C 
.data
extern func_check_1650:dword
extern target_next:dword
.code

sub_hook proc near

pushfd
pushad

push edx
call func_check_1650

popad
popfd

mov esi,[edx]
mov ebx,esi
mov [ebp-08],esi

jmp target_next

retn

sub_hook endp

END