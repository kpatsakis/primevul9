static inline void show_node(struct zone *zone)
{
	if (IS_ENABLED(CONFIG_NUMA))
		printk("Node %d ", zone_to_nid(zone));
}