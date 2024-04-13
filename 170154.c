static int perf_ftrace_function_unregister(struct perf_event *event)
{
	struct ftrace_ops *ops = &event->ftrace_ops;
	return unregister_ftrace_function(ops);
}