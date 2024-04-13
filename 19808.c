seq_hlist_next_percpu(void *v, struct hlist_head __percpu *head,
			int *cpu, loff_t *pos)
{
	struct hlist_node *node = v;

	++*pos;

	if (node->next)
		return node->next;

	for (*cpu = cpumask_next(*cpu, cpu_possible_mask); *cpu < nr_cpu_ids;
	     *cpu = cpumask_next(*cpu, cpu_possible_mask)) {
		struct hlist_head *bucket = per_cpu_ptr(head, *cpu);

		if (!hlist_empty(bucket))
			return bucket->first;
	}
	return NULL;
}