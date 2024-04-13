static inline int nested_symlink(struct path *path, struct nameidata *nd)
{
	int res;

	if (unlikely(current->link_count >= MAX_NESTED_LINKS)) {
		path_put_conditional(path, nd);
		path_put(&nd->path);
		return -ELOOP;
	}
	BUG_ON(nd->depth >= MAX_NESTED_LINKS);

	nd->depth++;
	current->link_count++;

	do {
		struct path link = *path;
		void *cookie;

		res = follow_link(&link, nd, &cookie);
		if (res)
			break;
		res = walk_component(nd, path, LOOKUP_FOLLOW);
		put_link(nd, &link, cookie);
	} while (res > 0);

	current->link_count--;
	nd->depth--;
	return res;
}