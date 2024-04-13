static int set_brk(unsigned long start, unsigned long end, int prot)
{
	start = ELF_PAGEALIGN(start);
	end = ELF_PAGEALIGN(end);
	if (end > start) {
		/*
		 * Map the last of the bss segment.
		 * If the header is requesting these pages to be
		 * executable, honour that (ppc32 needs this).
		 */
		int error = vm_brk_flags(start, end - start,
				prot & PROT_EXEC ? VM_EXEC : 0);
		if (error)
			return error;
	}
	current->mm->start_brk = current->mm->brk = end;
	return 0;
}