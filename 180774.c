static int __init fail_page_alloc_debugfs(void)
{
	umode_t mode = S_IFREG | S_IRUSR | S_IWUSR;
	struct dentry *dir;

	dir = fault_create_debugfs_attr("fail_page_alloc", NULL,
					&fail_page_alloc.attr);
	if (IS_ERR(dir))
		return PTR_ERR(dir);

	if (!debugfs_create_bool("ignore-gfp-wait", mode, dir,
				&fail_page_alloc.ignore_gfp_reclaim))
		goto fail;
	if (!debugfs_create_bool("ignore-gfp-highmem", mode, dir,
				&fail_page_alloc.ignore_gfp_highmem))
		goto fail;
	if (!debugfs_create_u32("min-order", mode, dir,
				&fail_page_alloc.min_order))
		goto fail;

	return 0;
fail:
	debugfs_remove_recursive(dir);

	return -ENOMEM;
}