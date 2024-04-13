static inline void copy_kernel_to_fpregs(union fpregs_state *fpstate)
{
	/*
	 * AMD K7/K8 CPUs don't save/restore FDP/FIP/FOP unless an exception is
	 * pending. Clear the x87 state here by setting it to fixed values.
	 * "m" is a random variable that should be in L1.
	 */
	if (unlikely(static_cpu_has_bug(X86_BUG_FXSAVE_LEAK))) {
		asm volatile(
			"fnclex\n\t"
			"emms\n\t"
			"fildl %P[addr]"	/* set F?P to defined value */
			: : [addr] "m" (fpstate));
	}

	__copy_kernel_to_fpregs(fpstate, -1);
}