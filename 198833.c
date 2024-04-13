static void inotify_remove_from_idr(struct fsnotify_group *group,
				    struct inotify_inode_mark *i_mark)
{
	struct idr *idr = &group->inotify_data.idr;
	spinlock_t *idr_lock = &group->inotify_data.idr_lock;
	struct inotify_inode_mark *found_i_mark = NULL;
	int wd;

	spin_lock(idr_lock);
	wd = i_mark->wd;

	/*
	 * does this i_mark think it is in the idr?  we shouldn't get called
	 * if it wasn't....
	 */
	if (wd == -1) {
		WARN_ONCE(1, "%s: i_mark=%p i_mark->wd=%d i_mark->group=%p\n",
			__func__, i_mark, i_mark->wd, i_mark->fsn_mark.group);
		goto out;
	}

	/* Lets look in the idr to see if we find it */
	found_i_mark = inotify_idr_find_locked(group, wd);
	if (unlikely(!found_i_mark)) {
		WARN_ONCE(1, "%s: i_mark=%p i_mark->wd=%d i_mark->group=%p\n",
			__func__, i_mark, i_mark->wd, i_mark->fsn_mark.group);
		goto out;
	}

	/*
	 * We found an mark in the idr at the right wd, but it's
	 * not the mark we were told to remove.  eparis seriously
	 * fucked up somewhere.
	 */
	if (unlikely(found_i_mark != i_mark)) {
		WARN_ONCE(1, "%s: i_mark=%p i_mark->wd=%d i_mark->group=%p "
			"found_i_mark=%p found_i_mark->wd=%d "
			"found_i_mark->group=%p\n", __func__, i_mark,
			i_mark->wd, i_mark->fsn_mark.group, found_i_mark,
			found_i_mark->wd, found_i_mark->fsn_mark.group);
		goto out;
	}

	/*
	 * One ref for being in the idr
	 * one ref grabbed by inotify_idr_find
	 */
	if (unlikely(refcount_read(&i_mark->fsn_mark.refcnt) < 2)) {
		printk(KERN_ERR "%s: i_mark=%p i_mark->wd=%d i_mark->group=%p\n",
			 __func__, i_mark, i_mark->wd, i_mark->fsn_mark.group);
		/* we can't really recover with bad ref cnting.. */
		BUG();
	}

	idr_remove(idr, wd);
	/* Removed from the idr, drop that ref. */
	fsnotify_put_mark(&i_mark->fsn_mark);
out:
	i_mark->wd = -1;
	spin_unlock(idr_lock);
	/* match the ref taken by inotify_idr_find_locked() */
	if (found_i_mark)
		fsnotify_put_mark(&found_i_mark->fsn_mark);
}