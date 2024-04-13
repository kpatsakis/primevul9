static struct fsnotify_group *inotify_new_group(unsigned int max_events)
{
	struct fsnotify_group *group;
	struct inotify_event_info *oevent;

	group = fsnotify_alloc_group(&inotify_fsnotify_ops);
	if (IS_ERR(group))
		return group;

	oevent = kmalloc(sizeof(struct inotify_event_info), GFP_KERNEL);
	if (unlikely(!oevent)) {
		fsnotify_destroy_group(group);
		return ERR_PTR(-ENOMEM);
	}
	group->overflow_event = &oevent->fse;
	fsnotify_init_event(group->overflow_event, NULL);
	oevent->mask = FS_Q_OVERFLOW;
	oevent->wd = -1;
	oevent->sync_cookie = 0;
	oevent->name_len = 0;

	group->max_events = max_events;
	group->memcg = get_mem_cgroup_from_mm(current->mm);

	spin_lock_init(&group->inotify_data.idr_lock);
	idr_init(&group->inotify_data.idr);
	group->inotify_data.ucounts = inc_ucount(current_user_ns(),
						 current_euid(),
						 UCOUNT_INOTIFY_INSTANCES);

	if (!group->inotify_data.ucounts) {
		fsnotify_destroy_group(group);
		return ERR_PTR(-EMFILE);
	}

	return group;
}