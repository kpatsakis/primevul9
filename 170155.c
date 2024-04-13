int perf_trace_init(struct perf_event *p_event)
{
	struct ftrace_event_call *tp_event;
	int event_id = p_event->attr.config;
	int ret = -EINVAL;

	mutex_lock(&event_mutex);
	list_for_each_entry(tp_event, &ftrace_events, list) {
		if (tp_event->event.type == event_id &&
		    tp_event->class && tp_event->class->reg &&
		    try_module_get(tp_event->mod)) {
			ret = perf_trace_event_init(tp_event, p_event);
			if (ret)
				module_put(tp_event->mod);
			break;
		}
	}
	mutex_unlock(&event_mutex);

	return ret;
}