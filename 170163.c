static void perf_trace_event_unreg(struct perf_event *p_event)
{
	struct ftrace_event_call *tp_event = p_event->tp_event;
	int i;

	if (--tp_event->perf_refcount > 0)
		goto out;

	tp_event->class->reg(tp_event, TRACE_REG_PERF_UNREGISTER, NULL);

	/*
	 * Ensure our callback won't be called anymore. The buffers
	 * will be freed after that.
	 */
	tracepoint_synchronize_unregister();

	free_percpu(tp_event->perf_events);
	tp_event->perf_events = NULL;

	if (!--total_ref_count) {
		for (i = 0; i < PERF_NR_CONTEXTS; i++) {
			free_percpu(perf_trace_buf[i]);
			perf_trace_buf[i] = NULL;
		}
	}
out:
	module_put(tp_event->mod);
}