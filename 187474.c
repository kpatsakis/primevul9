static void notify_one(struct mdesc_notifier_client *p,
		       struct mdesc_handle *old_hp,
		       struct mdesc_handle *new_hp)
{
	invoke_on_missing(p->node_name, old_hp, new_hp, p->remove);
	invoke_on_missing(p->node_name, new_hp, old_hp, p->add);
}