static inline bool smap_violation(int error_code, struct pt_regs *regs)
{
	if (!IS_ENABLED(CONFIG_X86_SMAP))
		return false;

	if (!static_cpu_has(X86_FEATURE_SMAP))
		return false;

	if (error_code & X86_PF_USER)
		return false;

	if (!user_mode(regs) && (regs->flags & X86_EFLAGS_AC))
		return false;

	return true;
}