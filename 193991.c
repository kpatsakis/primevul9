void blk_unregister_queue(struct gendisk *disk)
{
	struct request_queue *q = disk->queue;

	if (WARN_ON(!q))
		return;

	/* Return early if disk->queue was never registered. */
	if (!test_bit(QUEUE_FLAG_REGISTERED, &q->queue_flags))
		return;

	/*
	 * Since sysfs_remove_dir() prevents adding new directory entries
	 * before removal of existing entries starts, protect against
	 * concurrent elv_iosched_store() calls.
	 */
	mutex_lock(&q->sysfs_lock);

	blk_queue_flag_clear(QUEUE_FLAG_REGISTERED, q);

	/*
	 * Remove the sysfs attributes before unregistering the queue data
	 * structures that can be modified through sysfs.
	 */
	if (queue_is_mq(q))
		blk_mq_unregister_dev(disk_to_dev(disk), q);
	mutex_unlock(&q->sysfs_lock);

	kobject_uevent(&q->kobj, KOBJ_REMOVE);
	kobject_del(&q->kobj);
	blk_trace_remove_sysfs(disk_to_dev(disk));

	mutex_lock(&q->sysfs_lock);
	if (q->elevator)
		elv_unregister_queue(q);
	mutex_unlock(&q->sysfs_lock);

	kobject_put(&disk_to_dev(disk)->kobj);
}