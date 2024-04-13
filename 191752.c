static int netlink_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &netlink_seq_ops,
				sizeof(struct nl_seq_iter));
}