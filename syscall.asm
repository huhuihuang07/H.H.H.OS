global SysCall

extern gCurrentTaskAddr

[section .text]
[bits 32]

; uint32_t SysCall(uint32_t type, uint32_t cmd, uint32_t param1, uint32_t param2)
SysCall:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx

	mov edx, dword [ebp + 20]
	mov ecx, dword [ebp + 16]
	mov ebx, dword [ebp + 12]
	mov eax, dword [ebp +  8]

	int 0x80

	mov ebx, dword [gCurrentTaskAddr]
	mov eax, dword [ebx + 0x2c]

	pop edx
	pop ecx
	pop ebx

	leave
	ret

; end of [secion .text]	