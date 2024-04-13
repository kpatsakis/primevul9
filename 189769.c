static int proc_pid_wchan(struct task_struct *task, char *buffer)
{
	unsigned long wchan;
	char symname[KSYM_NAME_LEN];

	wchan = get_wchan(task);

	if (lookup_symbol_name(wchan, symname) < 0)
		return sprintf(buffer, "%lu", wchan);
	else
		return sprintf(buffer, "%s", symname);
}