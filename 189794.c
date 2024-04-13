static int lstats_show_proc(struct seq_file *m, void *v)
{
	int i;
	struct task_struct *task = m->private;
	seq_puts(m, "Latency Top version : v0.1\n");

	for (i = 0; i < 32; i++) {
		if (task->latency_record[i].backtrace[0]) {
			int q;
			seq_printf(m, "%i %li %li ",
				task->latency_record[i].count,
				task->latency_record[i].time,
				task->latency_record[i].max);
			for (q = 0; q < LT_BACKTRACEDEPTH; q++) {
				char sym[KSYM_NAME_LEN];
				char *c;
				if (!task->latency_record[i].backtrace[q])
					break;
				if (task->latency_record[i].backtrace[q] == ULONG_MAX)
					break;
				sprint_symbol(sym, task->latency_record[i].backtrace[q]);
				c = strchr(sym, '+');
				if (c)
					*c = 0;
				seq_printf(m, "%s ", sym);
			}
			seq_printf(m, "\n");
		}

	}
	return 0;
}