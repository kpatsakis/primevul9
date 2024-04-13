static int proc_pid_schedstat(struct task_struct *task, char *buffer)
{
	return sprintf(buffer, "%llu %llu %lu\n",
			task->sched_info.cpu_time,
			task->sched_info.run_delay,
			task->sched_info.pcount);
}