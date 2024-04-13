static void mce_wrmsrl(u32 msr, u64 v)
{
	if (__this_cpu_read(injectm.finished)) {
		int offset = msr_to_offset(msr);

		if (offset >= 0)
			*(u64 *)((char *)this_cpu_ptr(&injectm) + offset) = v;
		return;
	}
	wrmsrl(msr, v);
}