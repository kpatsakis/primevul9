static int intel_pmu_pebs_fixup_ip(struct pt_regs *regs)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	unsigned long from = cpuc->lbr_entries[0].from;
	unsigned long old_to, to = cpuc->lbr_entries[0].to;
	unsigned long ip = regs->ip;
	int is_64bit = 0;
	void *kaddr;
	int size;

	/*
	 * We don't need to fixup if the PEBS assist is fault like
	 */
	if (!x86_pmu.intel_cap.pebs_trap)
		return 1;

	/*
	 * No LBR entry, no basic block, no rewinding
	 */
	if (!cpuc->lbr_stack.nr || !from || !to)
		return 0;

	/*
	 * Basic blocks should never cross user/kernel boundaries
	 */
	if (kernel_ip(ip) != kernel_ip(to))
		return 0;

	/*
	 * unsigned math, either ip is before the start (impossible) or
	 * the basic block is larger than 1 page (sanity)
	 */
	if ((ip - to) > PEBS_FIXUP_SIZE)
		return 0;

	/*
	 * We sampled a branch insn, rewind using the LBR stack
	 */
	if (ip == to) {
		set_linear_ip(regs, from);
		return 1;
	}

	size = ip - to;
	if (!kernel_ip(ip)) {
		int bytes;
		u8 *buf = this_cpu_read(insn_buffer);

		/* 'size' must fit our buffer, see above */
		bytes = copy_from_user_nmi(buf, (void __user *)to, size);
		if (bytes != 0)
			return 0;

		kaddr = buf;
	} else {
		kaddr = (void *)to;
	}

	do {
		struct insn insn;

		old_to = to;

#ifdef CONFIG_X86_64
		is_64bit = kernel_ip(to) || any_64bit_mode(regs);
#endif
		insn_init(&insn, kaddr, size, is_64bit);
		insn_get_length(&insn);
		/*
		 * Make sure there was not a problem decoding the
		 * instruction and getting the length.  This is
		 * doubly important because we have an infinite
		 * loop if insn.length=0.
		 */
		if (!insn.length)
			break;

		to += insn.length;
		kaddr += insn.length;
		size -= insn.length;
	} while (to < ip);

	if (to == ip) {
		set_linear_ip(regs, old_to);
		return 1;
	}

	/*
	 * Even though we decoded the basic block, the instruction stream
	 * never matched the given IP, either the TO or the IP got corrupted.
	 */
	return 0;
}