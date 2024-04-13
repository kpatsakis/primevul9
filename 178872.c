static void dccp_feat_entry_destructor(struct dccp_feat_entry *entry)
{
	if (entry != NULL) {
		dccp_feat_val_destructor(entry->feat_num, &entry->val);
		kfree(entry);
	}
}