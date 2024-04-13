static inline void get_inline_info(struct inode *inode, struct f2fs_inode *ri)
{
	struct f2fs_inode_info *fi = F2FS_I(inode);

	if (ri->i_inline & F2FS_INLINE_XATTR)
		set_bit(FI_INLINE_XATTR, &fi->flags);
	if (ri->i_inline & F2FS_INLINE_DATA)
		set_bit(FI_INLINE_DATA, &fi->flags);
	if (ri->i_inline & F2FS_INLINE_DENTRY)
		set_bit(FI_INLINE_DENTRY, &fi->flags);
	if (ri->i_inline & F2FS_DATA_EXIST)
		set_bit(FI_DATA_EXIST, &fi->flags);
	if (ri->i_inline & F2FS_INLINE_DOTS)
		set_bit(FI_INLINE_DOTS, &fi->flags);
	if (ri->i_inline & F2FS_EXTRA_ATTR)
		set_bit(FI_EXTRA_ATTR, &fi->flags);
	if (ri->i_inline & F2FS_PIN_FILE)
		set_bit(FI_PIN_FILE, &fi->flags);
}