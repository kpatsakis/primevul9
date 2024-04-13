static inline void fpregs_activate(struct fpu *fpu)
{
	this_cpu_write(fpu_fpregs_owner_ctx, fpu);
	trace_x86_fpu_regs_activated(fpu);
}