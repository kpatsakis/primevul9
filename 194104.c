queue_attr_show(struct kobject *kobj, struct attribute *attr, char *page)
{
	struct queue_sysfs_entry *entry = to_queue(attr);
	struct request_queue *q =
		container_of(kobj, struct request_queue, kobj);
	ssize_t res;

	if (!entry->show)
		return -EIO;
	mutex_lock(&q->sysfs_lock);
	if (blk_queue_dying(q)) {
		mutex_unlock(&q->sysfs_lock);
		return -ENOENT;
	}
	res = entry->show(q, page);
	mutex_unlock(&q->sysfs_lock);
	return res;
}