static void enqueue_timer(struct timer_base *base, struct timer_list *timer,
			  unsigned int idx)
{
	hlist_add_head(&timer->entry, base->vectors + idx);
	__set_bit(idx, base->pending_map);
	timer_set_idx(timer, idx);

	trace_timer_start(timer, timer->expires, timer->flags);
}