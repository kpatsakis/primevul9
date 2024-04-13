static void __init of_unittest_check_phandles(void)
{
	struct device_node *np;
	struct node_hash *nh;
	struct hlist_node *tmp;
	int i, dup_count = 0, phandle_count = 0;

	for_each_of_allnodes(np) {
		if (!np->phandle)
			continue;

		hash_for_each_possible(phandle_ht, nh, node, np->phandle) {
			if (nh->np->phandle == np->phandle) {
				pr_info("Duplicate phandle! %i used by %pOF and %pOF\n",
					np->phandle, nh->np, np);
				dup_count++;
				break;
			}
		}

		nh = kzalloc(sizeof(*nh), GFP_KERNEL);
		if (!nh)
			return;

		nh->np = np;
		hash_add(phandle_ht, &nh->node, np->phandle);
		phandle_count++;
	}
	unittest(dup_count == 0, "Found %i duplicates in %i phandles\n",
		 dup_count, phandle_count);

	/* Clean up */
	hash_for_each_safe(phandle_ht, i, tmp, nh, node) {
		hash_del(&nh->node);
		kfree(nh);
	}
}