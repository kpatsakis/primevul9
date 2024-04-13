static long inotify_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	struct fsnotify_group *group;
	struct fsnotify_event *fsn_event;
	void __user *p;
	int ret = -ENOTTY;
	size_t send_len = 0;

	group = file->private_data;
	p = (void __user *) arg;

	pr_debug("%s: group=%p cmd=%u\n", __func__, group, cmd);

	switch (cmd) {
	case FIONREAD:
		spin_lock(&group->notification_lock);
		list_for_each_entry(fsn_event, &group->notification_list,
				    list) {
			send_len += sizeof(struct inotify_event);
			send_len += round_event_name_len(fsn_event);
		}
		spin_unlock(&group->notification_lock);
		ret = put_user(send_len, (int __user *) p);
		break;
#ifdef CONFIG_CHECKPOINT_RESTORE
	case INOTIFY_IOC_SETNEXTWD:
		ret = -EINVAL;
		if (arg >= 1 && arg <= INT_MAX) {
			struct inotify_group_private_data *data;

			data = &group->inotify_data;
			spin_lock(&data->idr_lock);
			idr_set_cursor(&data->idr, (unsigned int)arg);
			spin_unlock(&data->idr_lock);
			ret = 0;
		}
		break;
#endif /* CONFIG_CHECKPOINT_RESTORE */
	}

	return ret;
}