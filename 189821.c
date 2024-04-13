void proc_flush_task(struct task_struct *task)
{
	int i;
	struct pid *pid, *tgid = NULL;
	struct upid *upid;

	pid = task_pid(task);
	if (thread_group_leader(task))
		tgid = task_tgid(task);

	for (i = 0; i <= pid->level; i++) {
		upid = &pid->numbers[i];
		proc_flush_task_mnt(upid->ns->proc_mnt, upid->nr,
			tgid ? tgid->numbers[i].nr : 0);
	}

	upid = &pid->numbers[pid->level];
	if (upid->nr == 1)
		pid_ns_release_proc(upid->ns);
}