static void oom_kill_process(struct oom_control *oc, const char *message)
{
	struct task_struct *p = oc->chosen;
	unsigned int points = oc->chosen_points;
	struct task_struct *victim = p;
	struct task_struct *child;
	struct task_struct *t;
	struct mm_struct *mm;
	unsigned int victim_points = 0;
	static DEFINE_RATELIMIT_STATE(oom_rs, DEFAULT_RATELIMIT_INTERVAL,
					      DEFAULT_RATELIMIT_BURST);
	bool can_oom_reap = true;

	/*
	 * If the task is already exiting, don't alarm the sysadmin or kill
	 * its children or threads, just give it access to memory reserves
	 * so it can die quickly
	 */
	task_lock(p);
	if (task_will_free_mem(p)) {
		mark_oom_victim(p);
		wake_oom_reaper(p);
		task_unlock(p);
		put_task_struct(p);
		return;
	}
	task_unlock(p);

	if (__ratelimit(&oom_rs))
		dump_header(oc, p);

	pr_err("%s: Kill process %d (%s) score %u or sacrifice child\n",
		message, task_pid_nr(p), p->comm, points);

	/*
	 * If any of p's children has a different mm and is eligible for kill,
	 * the one with the highest oom_badness() score is sacrificed for its
	 * parent.  This attempts to lose the minimal amount of work done while
	 * still freeing memory.
	 */
	read_lock(&tasklist_lock);
	for_each_thread(p, t) {
		list_for_each_entry(child, &t->children, sibling) {
			unsigned int child_points;

			if (process_shares_mm(child, p->mm))
				continue;
			/*
			 * oom_badness() returns 0 if the thread is unkillable
			 */
			child_points = oom_badness(child,
				oc->memcg, oc->nodemask, oc->totalpages);
			if (child_points > victim_points) {
				put_task_struct(victim);
				victim = child;
				victim_points = child_points;
				get_task_struct(victim);
			}
		}
	}
	read_unlock(&tasklist_lock);

	p = find_lock_task_mm(victim);
	if (!p) {
		put_task_struct(victim);
		return;
	} else if (victim != p) {
		get_task_struct(p);
		put_task_struct(victim);
		victim = p;
	}

	/* Get a reference to safely compare mm after task_unlock(victim) */
	mm = victim->mm;
	mmgrab(mm);

	/* Raise event before sending signal: task reaper must see this */
	count_vm_event(OOM_KILL);
	count_memcg_event_mm(mm, OOM_KILL);

	/*
	 * We should send SIGKILL before granting access to memory reserves
	 * in order to prevent the OOM victim from depleting the memory
	 * reserves from the user space under its control.
	 */
	do_send_sig_info(SIGKILL, SEND_SIG_FORCED, victim, true);
	mark_oom_victim(victim);
	pr_err("Killed process %d (%s) total-vm:%lukB, anon-rss:%lukB, file-rss:%lukB, shmem-rss:%lukB\n",
		task_pid_nr(victim), victim->comm, K(victim->mm->total_vm),
		K(get_mm_counter(victim->mm, MM_ANONPAGES)),
		K(get_mm_counter(victim->mm, MM_FILEPAGES)),
		K(get_mm_counter(victim->mm, MM_SHMEMPAGES)));
	task_unlock(victim);

	/*
	 * Kill all user processes sharing victim->mm in other thread groups, if
	 * any.  They don't get access to memory reserves, though, to avoid
	 * depletion of all memory.  This prevents mm->mmap_sem livelock when an
	 * oom killed thread cannot exit because it requires the semaphore and
	 * its contended by another thread trying to allocate memory itself.
	 * That thread will now get access to memory reserves since it has a
	 * pending fatal signal.
	 */
	rcu_read_lock();
	for_each_process(p) {
		if (!process_shares_mm(p, mm))
			continue;
		if (same_thread_group(p, victim))
			continue;
		if (is_global_init(p)) {
			can_oom_reap = false;
			set_bit(MMF_OOM_SKIP, &mm->flags);
			pr_info("oom killer %d (%s) has mm pinned by %d (%s)\n",
					task_pid_nr(victim), victim->comm,
					task_pid_nr(p), p->comm);
			continue;
		}
		/*
		 * No use_mm() user needs to read from the userspace so we are
		 * ok to reap it.
		 */
		if (unlikely(p->flags & PF_KTHREAD))
			continue;
		do_send_sig_info(SIGKILL, SEND_SIG_FORCED, p, true);
	}
	rcu_read_unlock();

	if (can_oom_reap)
		wake_oom_reaper(victim);

	mmdrop(mm);
	put_task_struct(victim);
}