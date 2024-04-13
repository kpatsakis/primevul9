blk_qc_t generic_make_request(struct bio *bio)
{
	/*
	 * bio_list_on_stack[0] contains bios submitted by the current
	 * make_request_fn.
	 * bio_list_on_stack[1] contains bios that were submitted before
	 * the current make_request_fn, but that haven't been processed
	 * yet.
	 */
	struct bio_list bio_list_on_stack[2];
	blk_qc_t ret = BLK_QC_T_NONE;

	if (!generic_make_request_checks(bio))
		goto out;

	/*
	 * We only want one ->make_request_fn to be active at a time, else
	 * stack usage with stacked devices could be a problem.  So use
	 * current->bio_list to keep a list of requests submited by a
	 * make_request_fn function.  current->bio_list is also used as a
	 * flag to say if generic_make_request is currently active in this
	 * task or not.  If it is NULL, then no make_request is active.  If
	 * it is non-NULL, then a make_request is active, and new requests
	 * should be added at the tail
	 */
	if (current->bio_list) {
		bio_list_add(&current->bio_list[0], bio);
		goto out;
	}

	/* following loop may be a bit non-obvious, and so deserves some
	 * explanation.
	 * Before entering the loop, bio->bi_next is NULL (as all callers
	 * ensure that) so we have a list with a single bio.
	 * We pretend that we have just taken it off a longer list, so
	 * we assign bio_list to a pointer to the bio_list_on_stack,
	 * thus initialising the bio_list of new bios to be
	 * added.  ->make_request() may indeed add some more bios
	 * through a recursive call to generic_make_request.  If it
	 * did, we find a non-NULL value in bio_list and re-enter the loop
	 * from the top.  In this case we really did just take the bio
	 * of the top of the list (no pretending) and so remove it from
	 * bio_list, and call into ->make_request() again.
	 */
	BUG_ON(bio->bi_next);
	bio_list_init(&bio_list_on_stack[0]);
	current->bio_list = bio_list_on_stack;
	do {
		struct request_queue *q = bio->bi_disk->queue;
		blk_mq_req_flags_t flags = bio->bi_opf & REQ_NOWAIT ?
			BLK_MQ_REQ_NOWAIT : 0;

		if (likely(blk_queue_enter(q, flags) == 0)) {
			struct bio_list lower, same;

			/* Create a fresh bio_list for all subordinate requests */
			bio_list_on_stack[1] = bio_list_on_stack[0];
			bio_list_init(&bio_list_on_stack[0]);
			ret = q->make_request_fn(q, bio);

			blk_queue_exit(q);

			/* sort new bios into those for a lower level
			 * and those for the same level
			 */
			bio_list_init(&lower);
			bio_list_init(&same);
			while ((bio = bio_list_pop(&bio_list_on_stack[0])) != NULL)
				if (q == bio->bi_disk->queue)
					bio_list_add(&same, bio);
				else
					bio_list_add(&lower, bio);
			/* now assemble so we handle the lowest level first */
			bio_list_merge(&bio_list_on_stack[0], &lower);
			bio_list_merge(&bio_list_on_stack[0], &same);
			bio_list_merge(&bio_list_on_stack[0], &bio_list_on_stack[1]);
		} else {
			if (unlikely(!blk_queue_dying(q) &&
					(bio->bi_opf & REQ_NOWAIT)))
				bio_wouldblock_error(bio);
			else
				bio_io_error(bio);
		}
		bio = bio_list_pop(&bio_list_on_stack[0]);
	} while (bio);
	current->bio_list = NULL; /* deactivate */

out:
	return ret;
}