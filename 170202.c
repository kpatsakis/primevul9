xfs_iunlink_free_item(
	void			*ptr,
	void			*arg)
{
	struct xfs_iunlink	*iu = ptr;
	bool			*freed_anything = arg;

	*freed_anything = true;
	kmem_free(iu);
}