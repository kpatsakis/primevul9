static void forget_original_parent(struct task_struct *father,
					struct list_head *dead)
{
	struct task_struct *p, *t, *reaper;

	if (unlikely(!list_empty(&father->ptraced)))
		exit_ptrace(father, dead);

	/* Can drop and reacquire tasklist_lock */
	reaper = find_child_reaper(father);
	if (list_empty(&father->children))
		return;

	reaper = find_new_reaper(father, reaper);
	list_for_each_entry(p, &father->children, sibling) {
		for_each_thread(p, t) {
			t->real_parent = reaper;
			BUG_ON((!t->ptrace) != (t->parent == father));
			if (likely(!t->ptrace))
				t->parent = t->real_parent;
			if (t->pdeath_signal)
				group_send_sig_info(t->pdeath_signal,
						    SEND_SIG_NOINFO, t,
						    PIDTYPE_TGID);
		}
		/*
		 * If this is a threaded reparent there is no need to
		 * notify anyone anything has happened.
		 */
		if (!same_thread_group(reaper, father))
			reparent_leader(father, p, dead);
	}
	list_splice_tail_init(&father->children, &reaper->children);
}