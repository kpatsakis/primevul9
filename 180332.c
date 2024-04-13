void pagefault_out_of_memory(void)
{
	struct oom_control oc = {
		.zonelist = NULL,
		.nodemask = NULL,
		.memcg = NULL,
		.gfp_mask = 0,
		.order = 0,
	};

	if (mem_cgroup_oom_synchronize(true))
		return;

	if (!mutex_trylock(&oom_lock))
		return;
	out_of_memory(&oc);
	mutex_unlock(&oom_lock);
}