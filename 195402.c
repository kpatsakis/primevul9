mptctl_fasync(int fd, struct file *filep, int mode)
{
	MPT_ADAPTER	*ioc;
	int ret;

	mutex_lock(&mpctl_mutex);
	list_for_each_entry(ioc, &ioc_list, list)
		ioc->aen_event_read_flag=0;

	ret = fasync_helper(fd, filep, mode, &async_queue);
	mutex_unlock(&mpctl_mutex);
	return ret;
}