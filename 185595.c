__rb_reserve_next(struct ring_buffer_per_cpu *cpu_buffer,
		  struct rb_event_info *info)
{
	struct ring_buffer_event *event;
	struct buffer_page *tail_page;
	unsigned long tail, write, w;
	bool a_ok;
	bool b_ok;

	/* Don't let the compiler play games with cpu_buffer->tail_page */
	tail_page = info->tail_page = READ_ONCE(cpu_buffer->tail_page);

 /*A*/	w = local_read(&tail_page->write) & RB_WRITE_MASK;
	barrier();
	b_ok = rb_time_read(&cpu_buffer->before_stamp, &info->before);
	a_ok = rb_time_read(&cpu_buffer->write_stamp, &info->after);
	barrier();
	info->ts = rb_time_stamp(cpu_buffer->buffer);

	if ((info->add_timestamp & RB_ADD_STAMP_ABSOLUTE)) {
		info->delta = info->ts;
	} else {
		/*
		 * If interrupting an event time update, we may need an
		 * absolute timestamp.
		 * Don't bother if this is the start of a new page (w == 0).
		 */
		if (unlikely(!a_ok || !b_ok || (info->before != info->after && w))) {
			info->add_timestamp |= RB_ADD_STAMP_FORCE | RB_ADD_STAMP_EXTEND;
			info->length += RB_LEN_TIME_EXTEND;
		} else {
			info->delta = info->ts - info->after;
			if (unlikely(test_time_stamp(info->delta))) {
				info->add_timestamp |= RB_ADD_STAMP_EXTEND;
				info->length += RB_LEN_TIME_EXTEND;
			}
		}
	}

 /*B*/	rb_time_set(&cpu_buffer->before_stamp, info->ts);

 /*C*/	write = local_add_return(info->length, &tail_page->write);

	/* set write to only the index of the write */
	write &= RB_WRITE_MASK;

	tail = write - info->length;

	/* See if we shot pass the end of this buffer page */
	if (unlikely(write > BUF_PAGE_SIZE)) {
		/* before and after may now different, fix it up*/
		b_ok = rb_time_read(&cpu_buffer->before_stamp, &info->before);
		a_ok = rb_time_read(&cpu_buffer->write_stamp, &info->after);
		if (a_ok && b_ok && info->before != info->after)
			(void)rb_time_cmpxchg(&cpu_buffer->before_stamp,
					      info->before, info->after);
		if (a_ok && b_ok)
			check_buffer(cpu_buffer, info, CHECK_FULL_PAGE);
		return rb_move_tail(cpu_buffer, tail, info);
	}

	if (likely(tail == w)) {
		u64 save_before;
		bool s_ok;

		/* Nothing interrupted us between A and C */
 /*D*/		rb_time_set(&cpu_buffer->write_stamp, info->ts);
		barrier();
 /*E*/		s_ok = rb_time_read(&cpu_buffer->before_stamp, &save_before);
		RB_WARN_ON(cpu_buffer, !s_ok);
		if (likely(!(info->add_timestamp &
			     (RB_ADD_STAMP_FORCE | RB_ADD_STAMP_ABSOLUTE))))
			/* This did not interrupt any time update */
			info->delta = info->ts - info->after;
		else
			/* Just use full timestamp for interrupting event */
			info->delta = info->ts;
		barrier();
		check_buffer(cpu_buffer, info, tail);
		if (unlikely(info->ts != save_before)) {
			/* SLOW PATH - Interrupted between C and E */

			a_ok = rb_time_read(&cpu_buffer->write_stamp, &info->after);
			RB_WARN_ON(cpu_buffer, !a_ok);

			/* Write stamp must only go forward */
			if (save_before > info->after) {
				/*
				 * We do not care about the result, only that
				 * it gets updated atomically.
				 */
				(void)rb_time_cmpxchg(&cpu_buffer->write_stamp,
						      info->after, save_before);
			}
		}
	} else {
		u64 ts;
		/* SLOW PATH - Interrupted between A and C */
		a_ok = rb_time_read(&cpu_buffer->write_stamp, &info->after);
		/* Was interrupted before here, write_stamp must be valid */
		RB_WARN_ON(cpu_buffer, !a_ok);
		ts = rb_time_stamp(cpu_buffer->buffer);
		barrier();
 /*E*/		if (write == (local_read(&tail_page->write) & RB_WRITE_MASK) &&
		    info->after < ts &&
		    rb_time_cmpxchg(&cpu_buffer->write_stamp,
				    info->after, ts)) {
			/* Nothing came after this event between C and E */
			info->delta = ts - info->after;
		} else {
			/*
			 * Interrupted between C and E:
			 * Lost the previous events time stamp. Just set the
			 * delta to zero, and this will be the same time as
			 * the event this event interrupted. And the events that
			 * came after this will still be correct (as they would
			 * have built their delta on the previous event.
			 */
			info->delta = 0;
		}
		info->ts = ts;
		info->add_timestamp &= ~RB_ADD_STAMP_FORCE;
	}

	/*
	 * If this is the first commit on the page, then it has the same
	 * timestamp as the page itself.
	 */
	if (unlikely(!tail && !(info->add_timestamp &
				(RB_ADD_STAMP_FORCE | RB_ADD_STAMP_ABSOLUTE))))
		info->delta = 0;

	/* We reserved something on the buffer */

	event = __rb_page_index(tail_page, tail);
	rb_update_event(cpu_buffer, event, info);

	local_inc(&tail_page->entries);

	/*
	 * If this is the first commit on the page, then update
	 * its timestamp.
	 */
	if (unlikely(!tail))
		tail_page->page->time_stamp = info->ts;

	/* account for these added bytes */
	local_add(info->length, &cpu_buffer->entries_bytes);

	return event;
}