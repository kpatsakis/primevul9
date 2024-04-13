static void fill_prstatus(struct elf_prstatus *prstatus,
		struct task_struct *p, long signr)
{
	prstatus->pr_info.si_signo = prstatus->pr_cursig = signr;
	prstatus->pr_sigpend = p->pending.signal.sig[0];
	prstatus->pr_sighold = p->blocked.sig[0];
	rcu_read_lock();
	prstatus->pr_ppid = task_pid_vnr(rcu_dereference(p->real_parent));
	rcu_read_unlock();
	prstatus->pr_pid = task_pid_vnr(p);
	prstatus->pr_pgrp = task_pgrp_vnr(p);
	prstatus->pr_sid = task_session_vnr(p);
	if (thread_group_leader(p)) {
		struct task_cputime cputime;

		/*
		 * This is the record for the group leader.  It shows the
		 * group-wide total, not its individual thread total.
		 */
		thread_group_cputime(p, &cputime);
		prstatus->pr_utime = ns_to_timeval(cputime.utime);
		prstatus->pr_stime = ns_to_timeval(cputime.stime);
	} else {
		u64 utime, stime;

		task_cputime(p, &utime, &stime);
		prstatus->pr_utime = ns_to_timeval(utime);
		prstatus->pr_stime = ns_to_timeval(stime);
	}

	prstatus->pr_cutime = ns_to_timeval(p->signal->cutime);
	prstatus->pr_cstime = ns_to_timeval(p->signal->cstime);
}