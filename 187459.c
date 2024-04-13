static void __init report_platform_properties(void)
{
	struct mdesc_handle *hp = mdesc_grab();
	u64 pn = mdesc_node_by_name(hp, MDESC_NODE_NULL, "platform");
	const char *s;
	const u64 *v;

	if (pn == MDESC_NODE_NULL) {
		prom_printf("No platform node in machine-description.\n");
		prom_halt();
	}

	s = mdesc_get_property(hp, pn, "banner-name", NULL);
	printk("PLATFORM: banner-name [%s]\n", s);
	s = mdesc_get_property(hp, pn, "name", NULL);
	printk("PLATFORM: name [%s]\n", s);

	v = mdesc_get_property(hp, pn, "hostid", NULL);
	if (v)
		printk("PLATFORM: hostid [%08llx]\n", *v);
	v = mdesc_get_property(hp, pn, "serial#", NULL);
	if (v)
		printk("PLATFORM: serial# [%08llx]\n", *v);
	v = mdesc_get_property(hp, pn, "stick-frequency", NULL);
	printk("PLATFORM: stick-frequency [%08llx]\n", *v);
	v = mdesc_get_property(hp, pn, "mac-address", NULL);
	if (v)
		printk("PLATFORM: mac-address [%llx]\n", *v);
	v = mdesc_get_property(hp, pn, "watchdog-resolution", NULL);
	if (v)
		printk("PLATFORM: watchdog-resolution [%llu ms]\n", *v);
	v = mdesc_get_property(hp, pn, "watchdog-max-timeout", NULL);
	if (v)
		printk("PLATFORM: watchdog-max-timeout [%llu ms]\n", *v);
	v = mdesc_get_property(hp, pn, "max-cpus", NULL);
	if (v) {
		max_cpus = *v;
		printk("PLATFORM: max-cpus [%llu]\n", max_cpus);
	}

#ifdef CONFIG_SMP
	{
		int max_cpu, i;

		if (v) {
			max_cpu = *v;
			if (max_cpu > NR_CPUS)
				max_cpu = NR_CPUS;
		} else {
			max_cpu = NR_CPUS;
		}
		for (i = 0; i < max_cpu; i++)
			set_cpu_possible(i, true);
	}
#endif

	mdesc_release(hp);
}