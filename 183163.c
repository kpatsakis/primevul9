static int aio_migratepage(struct address_space *mapping, struct page *new,
			struct page *old, enum migrate_mode mode)
{
	struct kioctx *ctx;
	unsigned long flags;
	pgoff_t idx;
	int rc;

	rc = 0;

	/* mapping->private_lock here protects against the kioctx teardown.  */
	spin_lock(&mapping->private_lock);
	ctx = mapping->private_data;
	if (!ctx) {
		rc = -EINVAL;
		goto out;
	}

	/* The ring_lock mutex.  The prevents aio_read_events() from writing
	 * to the ring's head, and prevents page migration from mucking in
	 * a partially initialized kiotx.
	 */
	if (!mutex_trylock(&ctx->ring_lock)) {
		rc = -EAGAIN;
		goto out;
	}

	idx = old->index;
	if (idx < (pgoff_t)ctx->nr_pages) {
		/* Make sure the old page hasn't already been changed */
		if (ctx->ring_pages[idx] != old)
			rc = -EAGAIN;
	} else
		rc = -EINVAL;

	if (rc != 0)
		goto out_unlock;

	/* Writeback must be complete */
	BUG_ON(PageWriteback(old));
	get_page(new);

	rc = migrate_page_move_mapping(mapping, new, old, NULL, mode, 1);
	if (rc != MIGRATEPAGE_SUCCESS) {
		put_page(new);
		goto out_unlock;
	}

	/* Take completion_lock to prevent other writes to the ring buffer
	 * while the old page is copied to the new.  This prevents new
	 * events from being lost.
	 */
	spin_lock_irqsave(&ctx->completion_lock, flags);
	migrate_page_copy(new, old);
	BUG_ON(ctx->ring_pages[idx] != old);
	ctx->ring_pages[idx] = new;
	spin_unlock_irqrestore(&ctx->completion_lock, flags);

	/* The old page is no longer accessible. */
	put_page(old);

out_unlock:
	mutex_unlock(&ctx->ring_lock);
out:
	spin_unlock(&mapping->private_lock);
	return rc;
}