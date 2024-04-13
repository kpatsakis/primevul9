int i40e_count_filters(struct i40e_vsi *vsi)
{
	struct i40e_mac_filter *f;
	struct hlist_node *h;
	int bkt;
	int cnt = 0;

	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist)
		++cnt;

	return cnt;
}