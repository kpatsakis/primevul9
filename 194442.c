static inline int copy_kernel_to_fxregs_err(struct fxregs_state *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		return kernel_insn_err(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else
		return kernel_insn_err(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));
}