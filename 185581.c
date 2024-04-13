static int kill_proc(struct task_struct *t, unsigned long addr, int trapno,
			unsigned long pfn, struct page *page, int flags)
{
	struct siginfo si;
	int ret;

	pr_err("Memory failure: %#lx: Killing %s:%d due to hardware memory corruption\n",
		pfn, t->comm, t->pid);
	si.si_signo = SIGBUS;
	si.si_errno = 0;
	si.si_addr = (void *)addr;
#ifdef __ARCH_SI_TRAPNO
	si.si_trapno = trapno;
#endif
	si.si_addr_lsb = compound_order(compound_head(page)) + PAGE_SHIFT;

	if ((flags & MF_ACTION_REQUIRED) && t->mm == current->mm) {
		si.si_code = BUS_MCEERR_AR;
		ret = force_sig_info(SIGBUS, &si, current);
	} else {
		/*
		 * Don't use force here, it's convenient if the signal
		 * can be temporarily blocked.
		 * This could cause a loop when the user sets SIGBUS
		 * to SIG_IGN, but hopefully no one will do that?
		 */
		si.si_code = BUS_MCEERR_AO;
		ret = send_sig_info(SIGBUS, &si, t);  /* synchronous? */
	}
	if (ret < 0)
		pr_info("Memory failure: Error sending signal to %s:%d: %d\n",
			t->comm, t->pid, ret);
	return ret;
}