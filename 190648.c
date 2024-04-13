static inline void make_dentry_ptr_inline(struct inode *inode,
					struct f2fs_dentry_ptr *d, void *t)
{
	int entry_cnt = NR_INLINE_DENTRY(inode);
	int bitmap_size = INLINE_DENTRY_BITMAP_SIZE(inode);
	int reserved_size = INLINE_RESERVED_SIZE(inode);

	d->inode = inode;
	d->max = entry_cnt;
	d->nr_bitmap = bitmap_size;
	d->bitmap = t;
	d->dentry = t + bitmap_size + reserved_size;
	d->filename = t + bitmap_size + reserved_size +
					SIZE_OF_DIR_ENTRY * entry_cnt;
}