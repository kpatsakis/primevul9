static __u32 get_reg(struct fast_pool *f, struct pt_regs *regs)
{
	__u32 *ptr = (__u32 *) regs;
	unsigned int idx;

	if (regs == NULL)
		return 0;
	idx = READ_ONCE(f->reg_idx);
	if (idx >= sizeof(struct pt_regs) / sizeof(__u32))
		idx = 0;
	ptr += idx++;
	WRITE_ONCE(f->reg_idx, idx);
	return *ptr;
}