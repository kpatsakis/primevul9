static void dump_buffer_page(struct buffer_data_page *bpage,
			     struct rb_event_info *info,
			     unsigned long tail)
{
	struct ring_buffer_event *event;
	u64 ts, delta;
	int e;

	ts = bpage->time_stamp;
	pr_warn("  [%lld] PAGE TIME STAMP\n", ts);

	for (e = 0; e < tail; e += rb_event_length(event)) {

		event = (struct ring_buffer_event *)(bpage->data + e);

		switch (event->type_len) {

		case RINGBUF_TYPE_TIME_EXTEND:
			delta = rb_event_time_stamp(event);
			ts += delta;
			pr_warn("  [%lld] delta:%lld TIME EXTEND\n", ts, delta);
			break;

		case RINGBUF_TYPE_TIME_STAMP:
			delta = rb_event_time_stamp(event);
			ts = delta;
			pr_warn("  [%lld] absolute:%lld TIME STAMP\n", ts, delta);
			break;

		case RINGBUF_TYPE_PADDING:
			ts += event->time_delta;
			pr_warn("  [%lld] delta:%d PADDING\n", ts, event->time_delta);
			break;

		case RINGBUF_TYPE_DATA:
			ts += event->time_delta;
			pr_warn("  [%lld] delta:%d\n", ts, event->time_delta);
			break;

		default:
			break;
		}
	}
}