int blk_register_queue(struct gendisk *disk)
{
	int ret;
	struct device *dev = disk_to_dev(disk);
	struct request_queue *q = disk->queue;

	if (WARN_ON(!q))
		return -ENXIO;

	WARN_ONCE(test_bit(QUEUE_FLAG_REGISTERED, &q->queue_flags),
		  "%s is registering an already registered queue\n",
		  kobject_name(&dev->kobj));
	blk_queue_flag_set(QUEUE_FLAG_REGISTERED, q);

	/*
	 * SCSI probing may synchronously create and destroy a lot of
	 * request_queues for non-existent devices.  Shutting down a fully
	 * functional queue takes measureable wallclock time as RCU grace
	 * periods are involved.  To avoid excessive latency in these
	 * cases, a request_queue starts out in a degraded mode which is
	 * faster to shut down and is made fully functional here as
	 * request_queues for non-existent devices never get registered.
	 */
	if (!blk_queue_init_done(q)) {
		blk_queue_flag_set(QUEUE_FLAG_INIT_DONE, q);
		percpu_ref_switch_to_percpu(&q->q_usage_counter);
	}

	ret = blk_trace_init_sysfs(dev);
	if (ret)
		return ret;

	/* Prevent changes through sysfs until registration is completed. */
	mutex_lock(&q->sysfs_lock);

	ret = kobject_add(&q->kobj, kobject_get(&dev->kobj), "%s", "queue");
	if (ret < 0) {
		blk_trace_remove_sysfs(dev);
		goto unlock;
	}

	ret = sysfs_create_group(&q->kobj, &queue_attr_group);
	if (ret) {
		blk_trace_remove_sysfs(dev);
		kobject_del(&q->kobj);
		kobject_put(&dev->kobj);
		goto unlock;
	}

	if (queue_is_mq(q)) {
		__blk_mq_register_dev(dev, q);
		blk_mq_debugfs_register(q);
	}

	kobject_uevent(&q->kobj, KOBJ_ADD);

	wbt_enable_default(q);

	blk_throtl_register_queue(q);

	if (q->elevator) {
		ret = elv_register_queue(q);
		if (ret) {
			mutex_unlock(&q->sysfs_lock);
			kobject_uevent(&q->kobj, KOBJ_REMOVE);
			kobject_del(&q->kobj);
			blk_trace_remove_sysfs(dev);
			kobject_put(&dev->kobj);
			return ret;
		}
	}
	ret = 0;
unlock:
	mutex_unlock(&q->sysfs_lock);
	return ret;
}