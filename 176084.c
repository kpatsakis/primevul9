static void f2fs_destroy_inode(struct inode *inode)
{
	call_rcu(&inode->i_rcu, f2fs_i_callback);
}