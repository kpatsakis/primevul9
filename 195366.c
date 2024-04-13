static void fill_sig_info_pkey(int si_signo, int si_code, siginfo_t *info,
		u32 *pkey)
{
	/* This is effectively an #ifdef */
	if (!boot_cpu_has(X86_FEATURE_OSPKE))
		return;

	/* Fault not from Protection Keys: nothing to do */
	if ((si_code != SEGV_PKUERR) || (si_signo != SIGSEGV))
		return;
	/*
	 * force_sig_info_fault() is called from a number of
	 * contexts, some of which have a VMA and some of which
	 * do not.  The X86_PF_PK handing happens after we have a
	 * valid VMA, so we should never reach this without a
	 * valid VMA.
	 */
	if (!pkey) {
		WARN_ONCE(1, "PKU fault with no VMA passed in");
		info->si_pkey = 0;
		return;
	}
	/*
	 * si_pkey should be thought of as a strong hint, but not
	 * absolutely guranteed to be 100% accurate because of
	 * the race explained above.
	 */
	info->si_pkey = *pkey;
}