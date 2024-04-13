static void perf_ftrace_function_enable(struct perf_event *event)
{
	ftrace_function_local_enable(&event->ftrace_ops);
}