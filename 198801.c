static inline struct gadget_info *to_gadget_info(struct config_item *item)
{
	 return container_of(to_config_group(item), struct gadget_info, group);
}