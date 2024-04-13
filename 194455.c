static inline void copy_kernel_to_fxregs(struct fxregs_state *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		kernel_insn(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else
		kernel_insn(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));
}