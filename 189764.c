static int proc_pid_io_accounting(struct task_struct *task, char *buffer)
{
	return sprintf(buffer,
#ifdef CONFIG_TASK_XACCT
			"rchar: %llu\n"
			"wchar: %llu\n"
			"syscr: %llu\n"
			"syscw: %llu\n"
#endif
			"read_bytes: %llu\n"
			"write_bytes: %llu\n"
			"cancelled_write_bytes: %llu\n",
#ifdef CONFIG_TASK_XACCT
			(unsigned long long)task->rchar,
			(unsigned long long)task->wchar,
			(unsigned long long)task->syscr,
			(unsigned long long)task->syscw,
#endif
			(unsigned long long)task->ioac.read_bytes,
			(unsigned long long)task->ioac.write_bytes,
			(unsigned long long)task->ioac.cancelled_write_bytes);
}