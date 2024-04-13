eb_reset(struct eb_objects *eb)
{
	if (eb->and >= 0)
		memset(eb->buckets, 0, (eb->and+1)*sizeof(struct hlist_head));
}