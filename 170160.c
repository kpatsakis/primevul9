static void perf_ftrace_function_disable(struct perf_event *event)
{
	ftrace_function_local_disable(&event->ftrace_ops);
}