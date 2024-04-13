static inline struct gadget_strings *to_gadget_strings(struct config_item *item)
{
	 return container_of(to_config_group(item), struct gadget_strings,
			 group);
}