static void htab_map_seq_show_elem(struct bpf_map *map, void *key,
				   struct seq_file *m)
{
	void *value;

	rcu_read_lock();

	value = htab_map_lookup_elem(map, key);
	if (!value) {
		rcu_read_unlock();
		return;
	}

	btf_type_seq_show(map->btf, map->btf_key_type_id, key, m);
	seq_puts(m, ": ");
	btf_type_seq_show(map->btf, map->btf_value_type_id, value, m);
	seq_puts(m, "\n");

	rcu_read_unlock();
}