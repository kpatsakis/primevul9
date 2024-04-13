void perf_trace_del(struct perf_event *p_event, int flags)
{
	struct ftrace_event_call *tp_event = p_event->tp_event;
	hlist_del_rcu(&p_event->hlist_entry);
	tp_event->class->reg(tp_event, TRACE_REG_PERF_DEL, p_event);
}