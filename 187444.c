static void get_mondo_data(struct mdesc_handle *hp, u64 mp,
			   struct trap_per_cpu *tb)
{
	static int printed;
	const u64 *val;

	val = mdesc_get_property(hp, mp, "q-cpu-mondo-#bits", NULL);
	get_one_mondo_bits(val, &tb->cpu_mondo_qmask, 7, ilog2(max_cpus * 2));

	val = mdesc_get_property(hp, mp, "q-dev-mondo-#bits", NULL);
	get_one_mondo_bits(val, &tb->dev_mondo_qmask, 7, 8);

	val = mdesc_get_property(hp, mp, "q-resumable-#bits", NULL);
	get_one_mondo_bits(val, &tb->resum_qmask, 6, 7);

	val = mdesc_get_property(hp, mp, "q-nonresumable-#bits", NULL);
	get_one_mondo_bits(val, &tb->nonresum_qmask, 2, 2);
	if (!printed++) {
		pr_info("SUN4V: Mondo queue sizes "
			"[cpu(%u) dev(%u) r(%u) nr(%u)]\n",
			tb->cpu_mondo_qmask + 1,
			tb->dev_mondo_qmask + 1,
			tb->resum_qmask + 1,
			tb->nonresum_qmask + 1);
	}
}