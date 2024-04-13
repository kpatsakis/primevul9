
static void ql3xxx_timer(struct timer_list *t)
{
	struct ql3_adapter *qdev = from_timer(qdev, t, adapter_timer);
	queue_delayed_work(qdev->workqueue, &qdev->link_state_work, 0);