static void mdesc_notify_clients(struct mdesc_handle *old_hp,
				 struct mdesc_handle *new_hp)
{
	struct mdesc_notifier_client *p = client_list;

	while (p) {
		notify_one(p, old_hp, new_hp);
		p = p->next;
	}
}