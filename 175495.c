static void htab_percpu_map_seq_show_elem(struct bpf_map *map, void *key,
					  struct seq_file *m)
{
	struct htab_elem *l;
	void __percpu *pptr;
	int cpu;

	rcu_read_lock();

	l = __htab_map_lookup_elem(map, key);
	if (!l) {
		rcu_read_unlock();
		return;
	}

	btf_type_seq_show(map->btf, map->btf_key_type_id, key, m);
	seq_puts(m, ": {\n");
	pptr = htab_elem_get_ptr(l, map->key_size);
	for_each_possible_cpu(cpu) {
		seq_printf(m, "\tcpu%d: ", cpu);
		btf_type_seq_show(map->btf, map->btf_value_type_id,
				  per_cpu_ptr(pptr, cpu), m);
		seq_puts(m, "\n");
	}
	seq_puts(m, "}\n");

	rcu_read_unlock();
}