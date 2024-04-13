void ext4_journal_abort_handle(const char *caller, unsigned int line,
			       const char *err_fn, struct buffer_head *bh,
			       handle_t *handle, int err)
{
	char nbuf[16];
	const char *errstr = ext4_decode_error(NULL, err, nbuf);

	BUG_ON(!ext4_handle_valid(handle));

	if (bh)
		BUFFER_TRACE(bh, "abort");

	if (!handle->h_err)
		handle->h_err = err;

	if (is_handle_aborted(handle))
		return;

	printk(KERN_ERR "EXT4-fs: %s:%d: aborting transaction: %s in %s\n",
	       caller, line, errstr, err_fn);

	jbd2_journal_abort_handle(handle);
}