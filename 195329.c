struct fs_context *fs_context_for_reconfigure(struct dentry *dentry,
					unsigned int sb_flags,
					unsigned int sb_flags_mask)
{
	return alloc_fs_context(dentry->d_sb->s_type, dentry, sb_flags,
				sb_flags_mask, FS_CONTEXT_FOR_RECONFIGURE);
}