static __always_inline void set_root(struct nameidata *nd)
{
	if (!nd->root.mnt)
		get_fs_root(current->fs, &nd->root);
}