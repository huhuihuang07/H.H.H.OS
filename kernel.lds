ENTRY(_start)

SECTIONS
{
	.text 0xa000:
	{
		*(.text)
	}
	.data : 
	{
		*(.data)
	}
	.bss :
	{
		*(.bss)
	}
}