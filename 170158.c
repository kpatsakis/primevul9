int perf_trace_add(struct perf_event *p_event, int flags)
{
	struct ftrace_event_call *tp_event = p_event->tp_event;
	struct hlist_head __percpu *pcpu_list;
	struct hlist_head *list;

	pcpu_list = tp_event->perf_events;
	if (WARN_ON_ONCE(!pcpu_list))
		return -EINVAL;

	if (!(flags & PERF_EF_START))
		p_event->hw.state = PERF_HES_STOPPED;

	list = this_cpu_ptr(pcpu_list);
	hlist_add_head_rcu(&p_event->hlist_entry, list);

	return tp_event->class->reg(tp_event, TRACE_REG_PERF_ADD, p_event);
}