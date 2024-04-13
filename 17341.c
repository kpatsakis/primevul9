static inline int lookup_last(struct nameidata *nd, struct path *path)
{
	if (nd->last_type == LAST_NORM && nd->last.name[nd->last.len])
		nd->flags |= LOOKUP_FOLLOW | LOOKUP_DIRECTORY;

	nd->flags &= ~LOOKUP_PARENT;
	return walk_component(nd, path, nd->flags & LOOKUP_FOLLOW);
}