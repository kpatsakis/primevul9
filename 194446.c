static inline int copy_fregs_to_user(struct fregs_state __user *fx)
{
	return user_insn(fnsave %[fx]; fwait,  [fx] "=m" (*fx), "m" (*fx));
}