sg_proc_init(void)
{
	int num_leaves = ARRAY_SIZE(sg_proc_leaf_arr);
	int k;

	sg_proc_sgp = proc_mkdir(sg_proc_sg_dirname, NULL);
	if (!sg_proc_sgp)
		return 1;
	for (k = 0; k < num_leaves; ++k) {
		const struct sg_proc_leaf *leaf = &sg_proc_leaf_arr[k];
		umode_t mask = leaf->fops->write ? S_IRUGO | S_IWUSR : S_IRUGO;
		proc_create(leaf->name, mask, sg_proc_sgp, leaf->fops);
	}
	return 0;
}