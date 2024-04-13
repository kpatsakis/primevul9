__kprobes void *perf_trace_buf_prepare(int size, unsigned short type,
				       struct pt_regs *regs, int *rctxp)
{
	struct trace_entry *entry;
	unsigned long flags;
	char *raw_data;
	int pc;

	BUILD_BUG_ON(PERF_MAX_TRACE_SIZE % sizeof(unsigned long));

	pc = preempt_count();

	*rctxp = perf_swevent_get_recursion_context();
	if (*rctxp < 0)
		return NULL;

	raw_data = this_cpu_ptr(perf_trace_buf[*rctxp]);

	/* zero the dead bytes from align to not leak stack to user */
	memset(&raw_data[size - sizeof(u64)], 0, sizeof(u64));

	entry = (struct trace_entry *)raw_data;
	local_save_flags(flags);
	tracing_generic_entry_update(entry, flags, pc);
	entry->type = type;

	return raw_data;
}