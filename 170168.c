static int perf_trace_event_init(struct ftrace_event_call *tp_event,
				 struct perf_event *p_event)
{
	int ret;

	ret = perf_trace_event_perm(tp_event, p_event);
	if (ret)
		return ret;

	ret = perf_trace_event_reg(tp_event, p_event);
	if (ret)
		return ret;

	ret = perf_trace_event_open(p_event);
	if (ret) {
		perf_trace_event_unreg(p_event);
		return ret;
	}

	return 0;
}