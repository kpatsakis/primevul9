static struct btrfs_device *__alloc_device(void)
{
	struct btrfs_device *dev;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return ERR_PTR(-ENOMEM);

	/*
	 * Preallocate a bio that's always going to be used for flushing device
	 * barriers and matches the device lifespan
	 */
	dev->flush_bio = bio_alloc_bioset(GFP_KERNEL, 0, NULL);
	if (!dev->flush_bio) {
		kfree(dev);
		return ERR_PTR(-ENOMEM);
	}

	INIT_LIST_HEAD(&dev->dev_list);
	INIT_LIST_HEAD(&dev->dev_alloc_list);
	INIT_LIST_HEAD(&dev->resized_list);

	spin_lock_init(&dev->io_lock);

	atomic_set(&dev->reada_in_flight, 0);
	atomic_set(&dev->dev_stats_ccnt, 0);
	btrfs_device_data_ordered_init(dev);
	INIT_RADIX_TREE(&dev->reada_zones, GFP_NOFS & ~__GFP_DIRECT_RECLAIM);
	INIT_RADIX_TREE(&dev->reada_extents, GFP_NOFS & ~__GFP_DIRECT_RECLAIM);

	return dev;
}