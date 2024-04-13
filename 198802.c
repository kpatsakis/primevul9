void unregister_gadget_item(struct config_item *item)
{
	struct gadget_info *gi = to_gadget_info(item);

	mutex_lock(&gi->lock);
	unregister_gadget(gi);
	mutex_unlock(&gi->lock);
}