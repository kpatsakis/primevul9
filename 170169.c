static void perf_trace_event_close(struct perf_event *p_event)
{
	struct ftrace_event_call *tp_event = p_event->tp_event;
	tp_event->class->reg(tp_event, TRACE_REG_PERF_CLOSE, p_event);
}