static inline void fpregs_deactivate(struct fpu *fpu)
{
	this_cpu_write(fpu_fpregs_owner_ctx, NULL);
	trace_x86_fpu_regs_deactivated(fpu);
}