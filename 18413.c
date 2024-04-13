static void process_random_ready_list(void)
{
	unsigned long flags;
	struct random_ready_callback *rdy, *tmp;

	spin_lock_irqsave(&random_ready_list_lock, flags);
	list_for_each_entry_safe(rdy, tmp, &random_ready_list, list) {
		struct module *owner = rdy->owner;

		list_del_init(&rdy->list);
		rdy->func(rdy);
		module_put(owner);
	}
	spin_unlock_irqrestore(&random_ready_list_lock, flags);
}