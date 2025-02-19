sg_proc_cleanup(void)
{
	int k;
	int num_leaves = ARRAY_SIZE(sg_proc_leaf_arr);

	if (!sg_proc_sgp)
		return;
	for (k = 0; k < num_leaves; ++k)
		remove_proc_entry(sg_proc_leaf_arr[k].name, sg_proc_sgp);
	remove_proc_entry(sg_proc_sg_dirname, NULL);
}