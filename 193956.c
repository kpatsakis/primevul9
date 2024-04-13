queue_attr_store(struct kobject *kobj, struct attribute *attr,
		    const char *page, size_t length)
{
	struct queue_sysfs_entry *entry = to_queue(attr);
	struct request_queue *q;
	ssize_t res;

	if (!entry->store)
		return -EIO;

	q = container_of(kobj, struct request_queue, kobj);
	mutex_lock(&q->sysfs_lock);
	if (blk_queue_dying(q)) {
		mutex_unlock(&q->sysfs_lock);
		return -ENOENT;
	}
	res = entry->store(q, page, length);
	mutex_unlock(&q->sysfs_lock);
	return res;
}