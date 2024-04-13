static void dump_header(struct oom_control *oc, struct task_struct *p)
{
	pr_warn("%s invoked oom-killer: gfp_mask=%#x(%pGg), nodemask=",
		current->comm, oc->gfp_mask, &oc->gfp_mask);
	if (oc->nodemask)
		pr_cont("%*pbl", nodemask_pr_args(oc->nodemask));
	else
		pr_cont("(null)");
	pr_cont(",  order=%d, oom_score_adj=%hd\n",
		oc->order, current->signal->oom_score_adj);
	if (!IS_ENABLED(CONFIG_COMPACTION) && oc->order)
		pr_warn("COMPACTION is disabled!!!\n");

	cpuset_print_current_mems_allowed();
	dump_stack();
	if (oc->memcg)
		mem_cgroup_print_oom_info(oc->memcg, p);
	else
		show_mem(SHOW_MEM_FILTER_NODES, oc->nodemask);
	if (sysctl_oom_dump_tasks)
		dump_tasks(oc->memcg, oc->nodemask);
}