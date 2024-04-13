static int perf_ftrace_function_register(struct perf_event *event)
{
	struct ftrace_ops *ops = &event->ftrace_ops;

	ops->flags |= FTRACE_OPS_FL_CONTROL;
	ops->func = perf_ftrace_function_call;
	return register_ftrace_function(ops);
}