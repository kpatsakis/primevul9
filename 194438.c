static inline void copy_kernel_to_fregs(struct fregs_state *fx)
{
	kernel_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}