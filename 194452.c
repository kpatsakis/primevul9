static inline void __copy_kernel_to_fpregs(union fpregs_state *fpstate, u64 mask)
{
	if (use_xsave()) {
		copy_kernel_to_xregs(&fpstate->xsave, mask);
	} else {
		if (use_fxsr())
			copy_kernel_to_fxregs(&fpstate->fxsave);
		else
			copy_kernel_to_fregs(&fpstate->fsave);
	}
}