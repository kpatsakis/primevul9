int ftrace_profile_set_filter(struct perf_event *event, int event_id,
			      char *filter_str)
{
	int err;
	struct event_filter *filter = NULL;
	struct trace_event_call *call;

	mutex_lock(&event_mutex);

	call = event->tp_event;

	err = -EINVAL;
	if (!call)
		goto out_unlock;

	err = -EEXIST;
	if (event->filter)
		goto out_unlock;

	err = create_filter(NULL, call, filter_str, false, &filter);
	if (err)
		goto free_filter;

	if (ftrace_event_is_function(call))
		err = ftrace_function_set_filter(event, filter);
	else
		event->filter = filter;

free_filter:
	if (err || ftrace_event_is_function(call))
		__free_filter(filter);

out_unlock:
	mutex_unlock(&event_mutex);

	return err;
}