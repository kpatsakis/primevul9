struct fs_context *fs_context_for_submount(struct file_system_type *type,
					   struct dentry *reference)
{
	return alloc_fs_context(type, reference, 0, 0, FS_CONTEXT_FOR_SUBMOUNT);
}