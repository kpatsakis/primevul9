static inline int copy_kernel_to_fregs_err(struct fregs_state *fx)
{
	return kernel_insn_err(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}