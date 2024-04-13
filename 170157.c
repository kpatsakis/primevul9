static int perf_trace_event_reg(struct ftrace_event_call *tp_event,
				struct perf_event *p_event)
{
	struct hlist_head __percpu *list;
	int ret = -ENOMEM;
	int cpu;

	p_event->tp_event = tp_event;
	if (tp_event->perf_refcount++ > 0)
		return 0;

	list = alloc_percpu(struct hlist_head);
	if (!list)
		goto fail;

	for_each_possible_cpu(cpu)
		INIT_HLIST_HEAD(per_cpu_ptr(list, cpu));

	tp_event->perf_events = list;

	if (!total_ref_count) {
		char __percpu *buf;
		int i;

		for (i = 0; i < PERF_NR_CONTEXTS; i++) {
			buf = (char __percpu *)alloc_percpu(perf_trace_t);
			if (!buf)
				goto fail;

			perf_trace_buf[i] = buf;
		}
	}

	ret = tp_event->class->reg(tp_event, TRACE_REG_PERF_REGISTER, NULL);
	if (ret)
		goto fail;

	total_ref_count++;
	return 0;

fail:
	if (!total_ref_count) {
		int i;

		for (i = 0; i < PERF_NR_CONTEXTS; i++) {
			free_percpu(perf_trace_buf[i]);
			perf_trace_buf[i] = NULL;
		}
	}

	if (!--tp_event->perf_refcount) {
		free_percpu(tp_event->perf_events);
		tp_event->perf_events = NULL;
	}

	return ret;
}