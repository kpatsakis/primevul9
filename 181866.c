static int notify_push(unsigned int event_type, u32 controller)
{
	struct capictr_event *event = kmalloc(sizeof(*event), GFP_ATOMIC);

	if (!event)
		return -ENOMEM;

	INIT_WORK(&event->work, do_notify_work);
	event->type = event_type;
	event->controller = controller;

	queue_work(kcapi_wq, &event->work);
	return 0;
}