void perf_trace_destroy(struct perf_event *p_event)
{
	mutex_lock(&event_mutex);
	perf_trace_event_close(p_event);
	perf_trace_event_unreg(p_event);
	mutex_unlock(&event_mutex);
}