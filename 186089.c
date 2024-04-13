static void __init of_unittest_check_tree_linkage(void)
{
	struct device_node *np;
	int allnode_count = 0, child_count;

	if (!of_root)
		return;

	for_each_of_allnodes(np)
		allnode_count++;
	child_count = of_unittest_check_node_linkage(of_root);

	unittest(child_count > 0, "Device node data structure is corrupted\n");
	unittest(child_count == allnode_count,
		 "allnodes list size (%i) doesn't match sibling lists size (%i)\n",
		 allnode_count, child_count);
	pr_debug("allnodes list size (%i); sibling lists size (%i)\n", allnode_count, child_count);
}