static int mounts_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = file->private_data;
	struct mnt_namespace *ns = m->private;
	put_mnt_ns(ns);
	return seq_release(inode, file);
}