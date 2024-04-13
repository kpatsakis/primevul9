void drain_all_pages(struct zone *zone)
{
	int cpu;

	/*
	 * Allocate in the BSS so we wont require allocation in
	 * direct reclaim path for CONFIG_CPUMASK_OFFSTACK=y
	 */
	static cpumask_t cpus_with_pcps;

	/*
	 * Make sure nobody triggers this path before mm_percpu_wq is fully
	 * initialized.
	 */
	if (WARN_ON_ONCE(!mm_percpu_wq))
		return;

	/* Workqueues cannot recurse */
	if (current->flags & PF_WQ_WORKER)
		return;

	/*
	 * Do not drain if one is already in progress unless it's specific to
	 * a zone. Such callers are primarily CMA and memory hotplug and need
	 * the drain to be complete when the call returns.
	 */
	if (unlikely(!mutex_trylock(&pcpu_drain_mutex))) {
		if (!zone)
			return;
		mutex_lock(&pcpu_drain_mutex);
	}

	/*
	 * We don't care about racing with CPU hotplug event
	 * as offline notification will cause the notified
	 * cpu to drain that CPU pcps and on_each_cpu_mask
	 * disables preemption as part of its processing
	 */
	for_each_online_cpu(cpu) {
		struct per_cpu_pageset *pcp;
		struct zone *z;
		bool has_pcps = false;

		if (zone) {
			pcp = per_cpu_ptr(zone->pageset, cpu);
			if (pcp->pcp.count)
				has_pcps = true;
		} else {
			for_each_populated_zone(z) {
				pcp = per_cpu_ptr(z->pageset, cpu);
				if (pcp->pcp.count) {
					has_pcps = true;
					break;
				}
			}
		}

		if (has_pcps)
			cpumask_set_cpu(cpu, &cpus_with_pcps);
		else
			cpumask_clear_cpu(cpu, &cpus_with_pcps);
	}

	for_each_cpu(cpu, &cpus_with_pcps) {
		struct work_struct *work = per_cpu_ptr(&pcpu_drain, cpu);
		INIT_WORK(work, drain_local_pages_wq);
		queue_work_on(cpu, mm_percpu_wq, work);
	}
	for_each_cpu(cpu, &cpus_with_pcps)
		flush_work(per_cpu_ptr(&pcpu_drain, cpu));

	mutex_unlock(&pcpu_drain_mutex);
}