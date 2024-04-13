static int nfqnl_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &nfqnl_seq_ops,
			sizeof(struct iter_state));
}