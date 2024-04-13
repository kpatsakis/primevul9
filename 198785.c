static void gadget_strings_attr_release(struct config_item *item)
{
	struct gadget_strings *gs = to_gadget_strings(item);

	kfree(gs->manufacturer);
	kfree(gs->product);
	kfree(gs->serialnumber);

	list_del(&gs->list);
	kfree(gs);
}