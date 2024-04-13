static inline void __fpu_invalidate_fpregs_state(struct fpu *fpu)
{
	fpu->last_cpu = -1;
}