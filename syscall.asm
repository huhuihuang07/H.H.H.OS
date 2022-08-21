global SysCall

[section .text]
[bits 32]

; u32 SysCall(u32 type, u32 cmd, u32 param1, u32 param2)
SysCall:
	push ebp
	mov ebp, esp

	mov edx, dword [ebp + 20]
	mov ecx, dword [ebp + 16]
	mov ebx, dword [ebp + 12]
	mov eax, dword [ebp +  8]

	int 0x80

	leave
	ret

; end of [secion .text]	