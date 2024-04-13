static struct dccp_feat_entry *dccp_feat_list_lookup(struct list_head *fn_list,
						     u8 feat_num, bool is_local)
{
	struct dccp_feat_entry *entry;

	list_for_each_entry(entry, fn_list, node) {
		if (entry->feat_num == feat_num && entry->is_local == is_local)
			return entry;
		else if (entry->feat_num > feat_num)
			break;
	}
	return NULL;
}