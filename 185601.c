static void check_buffer(struct ring_buffer_per_cpu *cpu_buffer,
			 struct rb_event_info *info,
			 unsigned long tail)
{
	struct ring_buffer_event *event;
	struct buffer_data_page *bpage;
	u64 ts, delta;
	bool full = false;
	int e;

	bpage = info->tail_page->page;

	if (tail == CHECK_FULL_PAGE) {
		full = true;
		tail = local_read(&bpage->commit);
	} else if (info->add_timestamp &
		   (RB_ADD_STAMP_FORCE | RB_ADD_STAMP_ABSOLUTE)) {
		/* Ignore events with absolute time stamps */
		return;
	}

	/*
	 * Do not check the first event (skip possible extends too).
	 * Also do not check if previous events have not been committed.
	 */
	if (tail <= 8 || tail > local_read(&bpage->commit))
		return;

	/*
	 * If this interrupted another event, 
	 */
	if (atomic_inc_return(this_cpu_ptr(&checking)) != 1)
		goto out;

	ts = bpage->time_stamp;

	for (e = 0; e < tail; e += rb_event_length(event)) {

		event = (struct ring_buffer_event *)(bpage->data + e);

		switch (event->type_len) {

		case RINGBUF_TYPE_TIME_EXTEND:
			delta = rb_event_time_stamp(event);
			ts += delta;
			break;

		case RINGBUF_TYPE_TIME_STAMP:
			delta = rb_event_time_stamp(event);
			ts = delta;
			break;

		case RINGBUF_TYPE_PADDING:
			if (event->time_delta == 1)
				break;
			fallthrough;
		case RINGBUF_TYPE_DATA:
			ts += event->time_delta;
			break;

		default:
			RB_WARN_ON(cpu_buffer, 1);
		}
	}
	if ((full && ts > info->ts) ||
	    (!full && ts + info->delta != info->ts)) {
		/* If another report is happening, ignore this one */
		if (atomic_inc_return(&ts_dump) != 1) {
			atomic_dec(&ts_dump);
			goto out;
		}
		atomic_inc(&cpu_buffer->record_disabled);
		/* There's some cases in boot up that this can happen */
		WARN_ON_ONCE(system_state != SYSTEM_BOOTING);
		pr_warn("[CPU: %d]TIME DOES NOT MATCH expected:%lld actual:%lld delta:%lld before:%lld after:%lld%s\n",
			cpu_buffer->cpu,
			ts + info->delta, info->ts, info->delta,
			info->before, info->after,
			full ? " (full)" : "");
		dump_buffer_page(bpage, info, tail);
		atomic_dec(&ts_dump);
		/* Do not re-enable checking */
		return;
	}
out:
	atomic_dec(this_cpu_ptr(&checking));
}