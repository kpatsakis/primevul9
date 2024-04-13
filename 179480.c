static void __unhash_process(struct task_struct *p, bool group_dead)
{
	nr_threads--;
	detach_pid(p, PIDTYPE_PID);
	if (group_dead) {
		detach_pid(p, PIDTYPE_TGID);
		detach_pid(p, PIDTYPE_PGID);
		detach_pid(p, PIDTYPE_SID);

		list_del_rcu(&p->tasks);
		list_del_init(&p->sibling);
		__this_cpu_dec(process_counts);
	}
	list_del_rcu(&p->thread_group);
	list_del_rcu(&p->thread_node);
}