elv_attr_store(struct kobject *kobj, struct attribute *attr,
	       const char *page, size_t length)
{
	struct elv_fs_entry *entry = to_elv(attr);
	struct elevator_queue *e;
	ssize_t error;

	if (!entry->store)
		return -EIO;

	e = container_of(kobj, struct elevator_queue, kobj);
	mutex_lock(&e->sysfs_lock);
	error = e->type ? entry->store(e, page, length) : -ENOENT;
	mutex_unlock(&e->sysfs_lock);
	return error;
}