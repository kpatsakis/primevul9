	      dccp_feat_entry_new(struct list_head *head, u8 feat, bool local)
{
	struct dccp_feat_entry *entry;

	list_for_each_entry(entry, head, node)
		if (entry->feat_num == feat && entry->is_local == local) {
			dccp_feat_val_destructor(entry->feat_num, &entry->val);
			return entry;
		} else if (entry->feat_num > feat) {
			head = &entry->node;
			break;
		}

	entry = kmalloc(sizeof(*entry), gfp_any());
	if (entry != NULL) {
		entry->feat_num = feat;
		entry->is_local = local;
		list_add_tail(&entry->node, head);
	}
	return entry;
}