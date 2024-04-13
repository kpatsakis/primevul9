static int kill_proc(struct to_kill *tk, unsigned long pfn, int flags)
{
	struct task_struct *t = tk->tsk;
	short addr_lsb = tk->size_shift;
	int ret;

	pr_err("Memory failure: %#lx: Killing %s:%d due to hardware memory corruption\n",
		pfn, t->comm, t->pid);

	if ((flags & MF_ACTION_REQUIRED) && t->mm == current->mm) {
		ret = force_sig_mceerr(BUS_MCEERR_AR, (void __user *)tk->addr,
				       addr_lsb, current);
	} else {
		/*
		 * Don't use force here, it's convenient if the signal
		 * can be temporarily blocked.
		 * This could cause a loop when the user sets SIGBUS
		 * to SIG_IGN, but hopefully no one will do that?
		 */
		ret = send_sig_mceerr(BUS_MCEERR_AO, (void __user *)tk->addr,
				      addr_lsb, t);  /* synchronous? */
	}
	if (ret < 0)
		pr_info("Memory failure: Error sending signal to %s:%d: %d\n",
			t->comm, t->pid, ret);
	return ret;
}