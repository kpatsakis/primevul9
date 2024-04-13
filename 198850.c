static int inotify_new_watch(struct fsnotify_group *group,
			     struct inode *inode,
			     u32 arg)
{
	struct inotify_inode_mark *tmp_i_mark;
	__u32 mask;
	int ret;
	struct idr *idr = &group->inotify_data.idr;
	spinlock_t *idr_lock = &group->inotify_data.idr_lock;

	mask = inotify_arg_to_mask(arg);

	tmp_i_mark = kmem_cache_alloc(inotify_inode_mark_cachep, GFP_KERNEL);
	if (unlikely(!tmp_i_mark))
		return -ENOMEM;

	fsnotify_init_mark(&tmp_i_mark->fsn_mark, group);
	tmp_i_mark->fsn_mark.mask = mask;
	tmp_i_mark->wd = -1;

	ret = inotify_add_to_idr(idr, idr_lock, tmp_i_mark);
	if (ret)
		goto out_err;

	/* increment the number of watches the user has */
	if (!inc_inotify_watches(group->inotify_data.ucounts)) {
		inotify_remove_from_idr(group, tmp_i_mark);
		ret = -ENOSPC;
		goto out_err;
	}

	/* we are on the idr, now get on the inode */
	ret = fsnotify_add_inode_mark_locked(&tmp_i_mark->fsn_mark, inode, 0);
	if (ret) {
		/* we failed to get on the inode, get off the idr */
		inotify_remove_from_idr(group, tmp_i_mark);
		goto out_err;
	}


	/* return the watch descriptor for this new mark */
	ret = tmp_i_mark->wd;

out_err:
	/* match the ref from fsnotify_init_mark() */
	fsnotify_put_mark(&tmp_i_mark->fsn_mark);

	return ret;
}