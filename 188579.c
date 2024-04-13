unsigned long arch_mmap_rnd(void)
{
	unsigned long rnd;

	if (mmap_is_ia32())
#ifdef CONFIG_COMPAT
		rnd = (unsigned long)get_random_int() & ((1 << mmap_rnd_compat_bits) - 1);
#else
		rnd = (unsigned long)get_random_int() & ((1 << mmap_rnd_bits) - 1);
#endif
	else
		rnd = (unsigned long)get_random_int() & ((1 << mmap_rnd_bits) - 1);

	return rnd << PAGE_SHIFT;
}