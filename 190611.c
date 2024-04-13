}

static inline void f2fs_set_encrypted_inode(struct inode *inode)
{
#ifdef CONFIG_FS_ENCRYPTION
	file_set_encrypt(inode);
	f2fs_set_inode_flags(inode);