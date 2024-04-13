int seq_release_private(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;

	kfree(seq->private);
	seq->private = NULL;
	return seq_release(inode, file);
}