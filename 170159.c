int perf_ftrace_event_register(struct ftrace_event_call *call,
			       enum trace_reg type, void *data)
{
	switch (type) {
	case TRACE_REG_REGISTER:
	case TRACE_REG_UNREGISTER:
		break;
	case TRACE_REG_PERF_REGISTER:
	case TRACE_REG_PERF_UNREGISTER:
		return 0;
	case TRACE_REG_PERF_OPEN:
		return perf_ftrace_function_register(data);
	case TRACE_REG_PERF_CLOSE:
		return perf_ftrace_function_unregister(data);
	case TRACE_REG_PERF_ADD:
		perf_ftrace_function_enable(data);
		return 0;
	case TRACE_REG_PERF_DEL:
		perf_ftrace_function_disable(data);
		return 0;
	}

	return -EINVAL;
}