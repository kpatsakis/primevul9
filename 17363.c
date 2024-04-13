static inline void put_link(struct nameidata *nd, struct path *link, void *cookie)
{
	struct inode *inode = link->dentry->d_inode;
	if (inode->i_op->put_link)
		inode->i_op->put_link(link->dentry, nd, cookie);
	path_put(link);
}