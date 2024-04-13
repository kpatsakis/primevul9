static int proc_pid_auxv(struct task_struct *task, char *buffer)
{
	int res = 0;
	struct mm_struct *mm = get_task_mm(task);
	if (mm) {
		unsigned int nwords = 0;
		do
			nwords += 2;
		while (mm->saved_auxv[nwords - 2] != 0); /* AT_NULL */
		res = nwords * sizeof(mm->saved_auxv[0]);
		if (res > PAGE_SIZE)
			res = PAGE_SIZE;
		memcpy(buffer, mm->saved_auxv, res);
		mmput(mm);
	}
	return res;
}