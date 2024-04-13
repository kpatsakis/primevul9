int percpu_pagelist_fraction_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	struct zone *zone;
	int old_percpu_pagelist_fraction;
	int ret;

	mutex_lock(&pcp_batch_high_lock);
	old_percpu_pagelist_fraction = percpu_pagelist_fraction;

	ret = proc_dointvec_minmax(table, write, buffer, length, ppos);
	if (!write || ret < 0)
		goto out;

	/* Sanity checking to avoid pcp imbalance */
	if (percpu_pagelist_fraction &&
	    percpu_pagelist_fraction < MIN_PERCPU_PAGELIST_FRACTION) {
		percpu_pagelist_fraction = old_percpu_pagelist_fraction;
		ret = -EINVAL;
		goto out;
	}

	/* No change? */
	if (percpu_pagelist_fraction == old_percpu_pagelist_fraction)
		goto out;

	for_each_populated_zone(zone) {
		unsigned int cpu;

		for_each_possible_cpu(cpu)
			pageset_set_high_and_batch(zone,
					per_cpu_ptr(zone->pageset, cpu));
	}
out:
	mutex_unlock(&pcp_batch_high_lock);
	return ret;
}