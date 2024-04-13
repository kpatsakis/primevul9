static noinline void btrfs_schedule_bio(struct btrfs_device *device,
					struct bio *bio)
{
	struct btrfs_fs_info *fs_info = device->fs_info;
	int should_queue = 1;
	struct btrfs_pending_bios *pending_bios;

	/* don't bother with additional async steps for reads, right now */
	if (bio_op(bio) == REQ_OP_READ) {
		btrfsic_submit_bio(bio);
		return;
	}

	WARN_ON(bio->bi_next);
	bio->bi_next = NULL;

	spin_lock(&device->io_lock);
	if (op_is_sync(bio->bi_opf))
		pending_bios = &device->pending_sync_bios;
	else
		pending_bios = &device->pending_bios;

	if (pending_bios->tail)
		pending_bios->tail->bi_next = bio;

	pending_bios->tail = bio;
	if (!pending_bios->head)
		pending_bios->head = bio;
	if (device->running_pending)
		should_queue = 0;

	spin_unlock(&device->io_lock);

	if (should_queue)
		btrfs_queue_work(fs_info->submit_workers, &device->work);
}