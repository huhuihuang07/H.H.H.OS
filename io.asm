global inb
global inw

global outb
global outw

[section .text]
[bits 32]

sleep:
%rep 3
	jmp $ + 2
%endrep	

	ret

inb:
	push ebp
	mov ebp, esp

	push edx

	mov edx, dword [ebp + 8]
	in al, dx

	call sleep

	pop edx

	leave
	ret

inw:
	push ebp
	mov ebp, esp

	push edx

	mov edx, dword [ebp + 8]
	in ax, dx

	call sleep

	pop edx

	leave
	ret

outb:
	push ebp
	mov ebp, esp

	push eax
	push edx

	mov edx, dword [ebp +  8]
	mov eax, dword [ebp + 12]

	out dx, al

	call sleep

	pop edx
	pop eax

	leave
	ret	

outw:
	push ebp
	mov ebp, esp

	push eax
	push edx

	mov edx, dword [ebp +  8]
	mov eax, dword [ebp + 12]

	out dx, ax

	call sleep

	pop edx
	pop eax

	leave
	ret		

; end of [section .text]