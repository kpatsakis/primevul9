static inline void switch_fpu_finish(struct fpu *new_fpu)
{
	u32 pkru_val = init_pkru_value;
	struct pkru_state *pk;

	if (!static_cpu_has(X86_FEATURE_FPU))
		return;

	set_thread_flag(TIF_NEED_FPU_LOAD);

	if (!cpu_feature_enabled(X86_FEATURE_OSPKE))
		return;

	/*
	 * PKRU state is switched eagerly because it needs to be valid before we
	 * return to userland e.g. for a copy_to_user() operation.
	 */
	if (current->mm) {
		pk = get_xsave_addr(&new_fpu->state.xsave, XFEATURE_PKRU);
		if (pk)
			pkru_val = pk->pkru;
	}
	__write_pkru(pkru_val);
}