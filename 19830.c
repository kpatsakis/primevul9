int seq_open(struct file *file, const struct seq_operations *op)
{
	struct seq_file *p;

	WARN_ON(file->private_data);

	p = kmem_cache_zalloc(seq_file_cache, GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	file->private_data = p;

	mutex_init(&p->lock);
	p->op = op;

	// No refcounting: the lifetime of 'p' is constrained
	// to the lifetime of the file.
	p->file = file;

	/*
	 * seq_files support lseek() and pread().  They do not implement
	 * write() at all, but we clear FMODE_PWRITE here for historical
	 * reasons.
	 *
	 * If a client of seq_files a) implements file.write() and b) wishes to
	 * support pwrite() then that client will need to implement its own
	 * file.open() which calls seq_open() and then sets FMODE_PWRITE.
	 */
	file->f_mode &= ~FMODE_PWRITE;
	return 0;
}