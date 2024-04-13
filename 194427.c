static inline int copy_fpregs_to_fpstate(struct fpu *fpu)
{
	if (likely(use_xsave())) {
		copy_xregs_to_kernel(&fpu->state.xsave);

		/*
		 * AVX512 state is tracked here because its use is
		 * known to slow the max clock speed of the core.
		 */
		if (fpu->state.xsave.header.xfeatures & XFEATURE_MASK_AVX512)
			fpu->avx512_timestamp = jiffies;
		return 1;
	}

	if (likely(use_fxsr())) {
		copy_fxregs_to_kernel(fpu);
		return 1;
	}

	/*
	 * Legacy FPU register saving, FNSAVE always clears FPU registers,
	 * so we have to mark them inactive:
	 */
	asm volatile("fnsave %[fp]; fwait" : [fp] "=m" (fpu->state.fsave));

	return 0;
}