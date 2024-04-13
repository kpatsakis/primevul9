xfs_attr_shortform_compare(const void *a, const void *b)
{
	xfs_attr_sf_sort_t *sa, *sb;

	sa = (xfs_attr_sf_sort_t *)a;
	sb = (xfs_attr_sf_sort_t *)b;
	if (sa->hash < sb->hash) {
		return(-1);
	} else if (sa->hash > sb->hash) {
		return(1);
	} else {
		return(sa->entno - sb->entno);
	}
}