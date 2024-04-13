blk_qc_t submit_bio(struct bio *bio)
{
	/*
	 * If it's a regular read/write or a barrier with data attached,
	 * go through the normal accounting stuff before submission.
	 */
	if (bio_has_data(bio)) {
		unsigned int count;

		if (unlikely(bio_op(bio) == REQ_OP_WRITE_SAME))
			count = queue_logical_block_size(bio->bi_disk->queue) >> 9;
		else
			count = bio_sectors(bio);

		if (op_is_write(bio_op(bio))) {
			count_vm_events(PGPGOUT, count);
		} else {
			task_io_account_read(bio->bi_iter.bi_size);
			count_vm_events(PGPGIN, count);
		}

		if (unlikely(block_dump)) {
			char b[BDEVNAME_SIZE];
			printk(KERN_DEBUG "%s(%d): %s block %Lu on %s (%u sectors)\n",
			current->comm, task_pid_nr(current),
				op_is_write(bio_op(bio)) ? "WRITE" : "READ",
				(unsigned long long)bio->bi_iter.bi_sector,
				bio_devname(bio, b), count);
		}
	}

	return generic_make_request(bio);
}