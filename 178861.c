void dccp_feat_list_purge(struct list_head *fn_list)
{
	struct dccp_feat_entry *entry, *next;

	list_for_each_entry_safe(entry, next, fn_list, node)
		dccp_feat_entry_destructor(entry);
	INIT_LIST_HEAD(fn_list);
}