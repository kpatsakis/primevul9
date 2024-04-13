static umode_t queue_attr_visible(struct kobject *kobj, struct attribute *attr,
				int n)
{
	struct request_queue *q =
		container_of(kobj, struct request_queue, kobj);

	if (attr == &queue_io_timeout_entry.attr &&
		(!q->mq_ops || !q->mq_ops->timeout))
			return 0;

	return attr->mode;
}