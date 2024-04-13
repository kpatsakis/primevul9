seq_hlist_start_percpu(struct hlist_head __percpu *head, int *cpu, loff_t pos)
{
	struct hlist_node *node;

	for_each_possible_cpu(*cpu) {
		hlist_for_each(node, per_cpu_ptr(head, *cpu)) {
			if (pos-- == 0)
				return node;
		}
	}
	return NULL;
}