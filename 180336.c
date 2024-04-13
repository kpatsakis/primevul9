static int do_brk(unsigned long addr, unsigned long len, struct list_head *uf)
{
	return do_brk_flags(addr, len, 0, uf);
}