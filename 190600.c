static inline void __mark_inode_dirty_flag(struct inode *inode,
						int flag, bool set)
{
	switch (flag) {
	case FI_INLINE_XATTR:
	case FI_INLINE_DATA:
	case FI_INLINE_DENTRY:
	case FI_NEW_INODE:
		if (set)
			return;
		/* fall through */
	case FI_DATA_EXIST:
	case FI_INLINE_DOTS:
	case FI_PIN_FILE:
		f2fs_mark_inode_dirty_sync(inode, true);
	}
}