static void dump_header(struct oom_control *oc, struct task_struct *p)
{
	pr_warn("%s invoked oom-killer: gfp_mask=%#x(%pGg), nodemask=%*pbl, order=%d, oom_score_adj=%hd\n",
		current->comm, oc->gfp_mask, &oc->gfp_mask,
		nodemask_pr_args(oc->nodemask), oc->order,
			current->signal->oom_score_adj);
	if (!IS_ENABLED(CONFIG_COMPACTION) && oc->order)
		pr_warn("COMPACTION is disabled!!!\n");

	cpuset_print_current_mems_allowed();
	dump_stack();
	if (is_memcg_oom(oc))
		mem_cgroup_print_oom_info(oc->memcg, p);
	else {
		show_mem(SHOW_MEM_FILTER_NODES, oc->nodemask);
		if (is_dump_unreclaim_slabs())
			dump_unreclaimable_slab();
	}
	if (sysctl_oom_dump_tasks)
		dump_tasks(oc->memcg, oc->nodemask);
}