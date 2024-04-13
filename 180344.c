static void select_bad_process(struct oom_control *oc)
{
	if (is_memcg_oom(oc))
		mem_cgroup_scan_tasks(oc->memcg, oom_evaluate_task, oc);
	else {
		struct task_struct *p;

		rcu_read_lock();
		for_each_process(p)
			if (oom_evaluate_task(p, oc))
				break;
		rcu_read_unlock();
	}

	oc->chosen_points = oc->chosen_points * 1000 / oc->totalpages;
}