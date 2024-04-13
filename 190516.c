}

static inline bool f2fs_may_encrypt(struct inode *inode)
{
#ifdef CONFIG_FS_ENCRYPTION
	umode_t mode = inode->i_mode;

	return (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode));
#else
	return false;