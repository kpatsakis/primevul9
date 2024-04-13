int vfs_follow_link(struct nameidata *nd, const char *link)
{
	return __vfs_follow_link(nd, link);
}