static inline void __cpu_invalidate_fpregs_state(void)
{
	__this_cpu_write(fpu_fpregs_owner_ctx, NULL);
}