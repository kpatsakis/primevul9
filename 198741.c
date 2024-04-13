static inline struct gadget_config_name *to_gadget_config_name(
		struct config_item *item)
{
	 return container_of(to_config_group(item), struct gadget_config_name,
			 group);
}