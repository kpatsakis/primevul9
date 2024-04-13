static void do_notify_work(struct work_struct *work)
{
	struct capictr_event *event =
		container_of(work, struct capictr_event, work);

	switch (event->type) {
	case CAPICTR_UP:
		notify_up(event->controller);
		break;
	case CAPICTR_DOWN:
		notify_down(event->controller);
		break;
	}

	kfree(event);
}