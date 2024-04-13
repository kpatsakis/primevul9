static void jit_fill_hole(void *area, unsigned int size)
{
	/* fill whole space with int3 instructions */
	memset(area, 0xcc, size);
}