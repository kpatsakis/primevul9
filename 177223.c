void sdma_seqfile_dump_cpu_list(struct seq_file *s,
				struct hfi1_devdata *dd,
				unsigned long cpuid)
{
	struct sdma_rht_node *rht_node;
	int i, j;

	rht_node = rhashtable_lookup_fast(dd->sdma_rht, &cpuid,
					  sdma_rht_params);
	if (!rht_node)
		return;

	seq_printf(s, "cpu%3lu: ", cpuid);
	for (i = 0; i < HFI1_MAX_VLS_SUPPORTED; i++) {
		if (!rht_node->map[i] || !rht_node->map[i]->ctr)
			continue;

		seq_printf(s, " vl%d: [", i);

		for (j = 0; j < rht_node->map[i]->ctr; j++) {
			if (!rht_node->map[i]->sde[j])
				continue;

			if (j > 0)
				seq_puts(s, ",");

			seq_printf(s, " sdma%2d",
				   rht_node->map[i]->sde[j]->this_idx);
		}
		seq_puts(s, " ]");
	}

	seq_puts(s, "\n");
}