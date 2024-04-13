static struct i40e_cloud_filter *i40e_find_cloud_filter(struct i40e_vsi *vsi,
							unsigned long *cookie)
{
	struct i40e_cloud_filter *filter = NULL;
	struct hlist_node *node2;

	hlist_for_each_entry_safe(filter, node2,
				  &vsi->back->cloud_filter_list, cloud_node)
		if (!memcmp(cookie, &filter->cookie, sizeof(filter->cookie)))
			return filter;
	return NULL;
}