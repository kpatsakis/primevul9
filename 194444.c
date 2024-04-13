static inline void copy_fxregs_to_kernel(struct fpu *fpu)
{
	if (IS_ENABLED(CONFIG_X86_32))
		asm volatile( "fxsave %[fx]" : [fx] "=m" (fpu->state.fxsave));
	else
		asm volatile("fxsaveq %[fx]" : [fx] "=m" (fpu->state.fxsave));
}