static int do_maps_open(struct inode *inode, struct file *file,
			struct seq_operations *ops)
{
	struct proc_maps_private *priv;
	int ret = -ENOMEM;
	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (priv) {
		priv->pid = proc_pid(inode);
		ret = seq_open(file, ops);
		if (!ret) {
			struct seq_file *m = file->private_data;
			m->private = priv;
		} else {
			kfree(priv);
		}
	}
	return ret;
}