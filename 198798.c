static inline struct os_desc *to_os_desc(struct config_item *item)
{
	return container_of(to_config_group(item), struct os_desc, group);
}