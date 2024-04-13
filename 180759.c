void __init mem_init_print_info(const char *str)
{
	unsigned long physpages, codesize, datasize, rosize, bss_size;
	unsigned long init_code_size, init_data_size;

	physpages = get_num_physpages();
	codesize = _etext - _stext;
	datasize = _edata - _sdata;
	rosize = __end_rodata - __start_rodata;
	bss_size = __bss_stop - __bss_start;
	init_data_size = __init_end - __init_begin;
	init_code_size = _einittext - _sinittext;

	/*
	 * Detect special cases and adjust section sizes accordingly:
	 * 1) .init.* may be embedded into .data sections
	 * 2) .init.text.* may be out of [__init_begin, __init_end],
	 *    please refer to arch/tile/kernel/vmlinux.lds.S.
	 * 3) .rodata.* may be embedded into .text or .data sections.
	 */
#define adj_init_size(start, end, size, pos, adj) \
	do { \
		if (start <= pos && pos < end && size > adj) \
			size -= adj; \
	} while (0)

	adj_init_size(__init_begin, __init_end, init_data_size,
		     _sinittext, init_code_size);
	adj_init_size(_stext, _etext, codesize, _sinittext, init_code_size);
	adj_init_size(_sdata, _edata, datasize, __init_begin, init_data_size);
	adj_init_size(_stext, _etext, codesize, __start_rodata, rosize);
	adj_init_size(_sdata, _edata, datasize, __start_rodata, rosize);

#undef	adj_init_size

	pr_info("Memory: %luK/%luK available (%luK kernel code, %luK rwdata, %luK rodata, %luK init, %luK bss, %luK reserved, %luK cma-reserved"
#ifdef	CONFIG_HIGHMEM
		", %luK highmem"
#endif
		"%s%s)\n",
		nr_free_pages() << (PAGE_SHIFT - 10),
		physpages << (PAGE_SHIFT - 10),
		codesize >> 10, datasize >> 10, rosize >> 10,
		(init_data_size + init_code_size) >> 10, bss_size >> 10,
		(physpages - totalram_pages - totalcma_pages) << (PAGE_SHIFT - 10),
		totalcma_pages << (PAGE_SHIFT - 10),
#ifdef	CONFIG_HIGHMEM
		totalhigh_pages << (PAGE_SHIFT - 10),
#endif
		str ? ", " : "", str ? str : "");
}