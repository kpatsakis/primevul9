static void __build_all_zonelists(void *data)
{
	int nid;
	int __maybe_unused cpu;
	pg_data_t *self = data;
	static DEFINE_SPINLOCK(lock);

	spin_lock(&lock);

#ifdef CONFIG_NUMA
	memset(node_load, 0, sizeof(node_load));
#endif

	/*
	 * This node is hotadded and no memory is yet present.   So just
	 * building zonelists is fine - no need to touch other nodes.
	 */
	if (self && !node_online(self->node_id)) {
		build_zonelists(self);
	} else {
		for_each_online_node(nid) {
			pg_data_t *pgdat = NODE_DATA(nid);

			build_zonelists(pgdat);
		}

#ifdef CONFIG_HAVE_MEMORYLESS_NODES
		/*
		 * We now know the "local memory node" for each node--
		 * i.e., the node of the first zone in the generic zonelist.
		 * Set up numa_mem percpu variable for on-line cpus.  During
		 * boot, only the boot cpu should be on-line;  we'll init the
		 * secondary cpus' numa_mem as they come on-line.  During
		 * node/memory hotplug, we'll fixup all on-line cpus.
		 */
		for_each_online_cpu(cpu)
			set_cpu_numa_mem(cpu, local_memory_node(cpu_to_node(cpu)));
#endif
	}

	spin_unlock(&lock);
}