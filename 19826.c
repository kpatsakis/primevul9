int seq_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = file->private_data;
	kvfree(m->buf);
	kmem_cache_free(seq_file_cache, m);
	return 0;
}