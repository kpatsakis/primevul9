void unlock_rename(struct dentry *p1, struct dentry *p2)
{
	mutex_unlock(&p1->d_inode->i_mutex);
	if (p1 != p2) {
		mutex_unlock(&p2->d_inode->i_mutex);
		mutex_unlock(&p1->d_inode->i_sb->s_vfs_rename_mutex);
	}
}