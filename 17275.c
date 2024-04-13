static inline int should_follow_link(struct inode *inode, int follow)
{
	if (unlikely(!(inode->i_opflags & IOP_NOFOLLOW))) {
		if (likely(inode->i_op->follow_link))
			return follow;

		/* This gets set once for the inode lifetime */
		spin_lock(&inode->i_lock);
		inode->i_opflags |= IOP_NOFOLLOW;
		spin_unlock(&inode->i_lock);
	}
	return 0;
}