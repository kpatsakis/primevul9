static inline struct gadget_info *os_desc_item_to_gadget_info(
		struct config_item *item)
{
	return to_gadget_info(to_os_desc(item)->group.cg_item.ci_parent);
}