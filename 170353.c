static int bpf_link_release(struct inode *inode, struct file *filp)
{
	struct bpf_link *link = filp->private_data;

	bpf_link_put(link);
	return 0;
}