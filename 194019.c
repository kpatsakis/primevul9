static inline bool bio_check_ro(struct bio *bio, struct hd_struct *part)
{
	const int op = bio_op(bio);

	if (part->policy && op_is_write(op)) {
		char b[BDEVNAME_SIZE];

		if (op_is_flush(bio->bi_opf) && !bio_sectors(bio))
			return false;

		WARN_ONCE(1,
		       "generic_make_request: Trying to write "
			"to read-only block-device %s (partno %d)\n",
			bio_devname(bio, b), part->partno);
		/* Older lvm-tools actually trigger this */
		return false;
	}

	return false;
}