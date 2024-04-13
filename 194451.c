static inline int copy_fxregs_to_user(struct fxregs_state __user *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		return user_insn(fxsave %[fx], [fx] "=m" (*fx), "m" (*fx));
	else
		return user_insn(fxsaveq %[fx], [fx] "=m" (*fx), "m" (*fx));

}