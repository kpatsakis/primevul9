static struct elevator_type *elevator_get(struct request_queue *q,
					  const char *name, bool try_loading)
{
	struct elevator_type *e;

	spin_lock(&elv_list_lock);

	e = elevator_find(name);
	if (!e && try_loading) {
		spin_unlock(&elv_list_lock);
		request_module("%s-iosched", name);
		spin_lock(&elv_list_lock);
		e = elevator_find(name);
	}

	if (e && !try_module_get(e->elevator_owner))
		e = NULL;

	spin_unlock(&elv_list_lock);
	return e;
}