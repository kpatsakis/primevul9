static void dump_tasks(struct mem_cgroup *memcg, const nodemask_t *nodemask)
{
	struct task_struct *p;
	struct task_struct *task;

	pr_info("[ pid ]   uid  tgid total_vm      rss nr_ptes nr_pmds swapents oom_score_adj name\n");
	rcu_read_lock();
	for_each_process(p) {
		if (oom_unkillable_task(p, memcg, nodemask))
			continue;

		task = find_lock_task_mm(p);
		if (!task) {
			/*
			 * This is a kthread or all of p's threads have already
			 * detached their mm's.  There's no need to report
			 * them; they can't be oom killed anyway.
			 */
			continue;
		}

		pr_info("[%5d] %5d %5d %8lu %8lu %7ld %7ld %8lu         %5hd %s\n",
			task->pid, from_kuid(&init_user_ns, task_uid(task)),
			task->tgid, task->mm->total_vm, get_mm_rss(task->mm),
			atomic_long_read(&task->mm->nr_ptes),
			mm_nr_pmds(task->mm),
			get_mm_counter(task->mm, MM_SWAPENTS),
			task->signal->oom_score_adj, task->comm);
		task_unlock(task);
	}
	rcu_read_unlock();
}