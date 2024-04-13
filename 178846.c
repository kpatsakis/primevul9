static inline void dccp_feat_list_pop(struct dccp_feat_entry *entry)
{
	list_del(&entry->node);
	dccp_feat_entry_destructor(entry);
}