static int inotify_release(struct inode *ignored, struct file *file)
{
	struct fsnotify_group *group = file->private_data;

	pr_debug("%s: group=%p\n", __func__, group);

	/* free this group, matching get was inotify_init->fsnotify_obtain_group */
	fsnotify_destroy_group(group);

	return 0;
}