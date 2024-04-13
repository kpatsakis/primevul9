ext4_xattr_handler(int name_index)
{
	const struct xattr_handler *handler = NULL;

	if (name_index > 0 && name_index < ARRAY_SIZE(ext4_xattr_handler_map))
		handler = ext4_xattr_handler_map[name_index];
	return handler;
}