static void kill_procs(struct list_head *to_kill, int forcekill, bool fail,
		unsigned long pfn, int flags)
{
	struct to_kill *tk, *next;

	list_for_each_entry_safe (tk, next, to_kill, nd) {
		if (forcekill) {
			/*
			 * In case something went wrong with munmapping
			 * make sure the process doesn't catch the
			 * signal and then access the memory. Just kill it.
			 */
			if (fail || tk->addr_valid == 0) {
				pr_err("Memory failure: %#lx: forcibly killing %s:%d because of failure to unmap corrupted page\n",
				       pfn, tk->tsk->comm, tk->tsk->pid);
				do_send_sig_info(SIGKILL, SEND_SIG_PRIV,
						 tk->tsk, PIDTYPE_PID);
			}

			/*
			 * In theory the process could have mapped
			 * something else on the address in-between. We could
			 * check for that, but we need to tell the
			 * process anyways.
			 */
			else if (kill_proc(tk, pfn, flags) < 0)
				pr_err("Memory failure: %#lx: Cannot send advisory machine check signal to %s:%d\n",
				       pfn, tk->tsk->comm, tk->tsk->pid);
		}
		put_task_struct(tk->tsk);
		kfree(tk);
	}
}