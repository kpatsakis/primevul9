static void gadgets_drop(struct config_group *group, struct config_item *item)
{
	config_item_put(item);
}