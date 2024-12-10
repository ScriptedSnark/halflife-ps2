void Sys_InitELF();
void __attribute__((optimize("O3"))) INVOKER();

int CompatibleCRCList[] = { 0xA880AE9B }; // SLUS_200.66 (got CRC from PCSX2 logs)

void init()
{
	Sys_InitELF();
}

// needs to be 03 because O0 ironically does some funky stuff with this function
void __attribute__((optimize("O3"))) INVOKER()
{
	asm("ei\n");
	asm("addiu $ra, -4\n");

	init();
}

int __attribute__((optimize("O3"))) main()
{
	return 0;
}