static inline int copy_user_to_fregs(struct fregs_state __user *fx)
{
	return user_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}