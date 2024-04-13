void mdesc_fill_in_cpu_data(cpumask_t *mask)
{
	struct mdesc_handle *hp;

	mdesc_iterate_over_cpus(fill_in_one_cpu, NULL, mask);

	hp = mdesc_grab();

	set_core_ids(hp);
	set_proc_ids(hp);
	set_sock_ids(hp);

	mdesc_release(hp);

	smp_fill_in_sib_core_maps();
}