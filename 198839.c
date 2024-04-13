static int inotify_update_watch(struct fsnotify_group *group, struct inode *inode, u32 arg)
{
	int ret = 0;

	mutex_lock(&group->mark_mutex);
	/* try to update and existing watch with the new arg */
	ret = inotify_update_existing_watch(group, inode, arg);
	/* no mark present, try to add a new one */
	if (ret == -ENOENT)
		ret = inotify_new_watch(group, inode, arg);
	mutex_unlock(&group->mark_mutex);

	return ret;
}