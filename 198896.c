static int process_system_preds(struct trace_subsystem_dir *dir,
				struct trace_array *tr,
				struct filter_parse_error *pe,
				char *filter_string)
{
	struct trace_event_file *file;
	struct filter_list *filter_item;
	struct event_filter *filter = NULL;
	struct filter_list *tmp;
	LIST_HEAD(filter_list);
	bool fail = true;
	int err;

	list_for_each_entry(file, &tr->events, list) {

		if (file->system != dir)
			continue;

		filter = kzalloc(sizeof(*filter), GFP_KERNEL);
		if (!filter)
			goto fail_mem;

		filter->filter_string = kstrdup(filter_string, GFP_KERNEL);
		if (!filter->filter_string)
			goto fail_mem;

		err = process_preds(file->event_call, filter_string, filter, pe);
		if (err) {
			filter_disable(file);
			parse_error(pe, FILT_ERR_BAD_SUBSYS_FILTER, 0);
			append_filter_err(tr, pe, filter);
		} else
			event_set_filtered_flag(file);


		filter_item = kzalloc(sizeof(*filter_item), GFP_KERNEL);
		if (!filter_item)
			goto fail_mem;

		list_add_tail(&filter_item->list, &filter_list);
		/*
		 * Regardless of if this returned an error, we still
		 * replace the filter for the call.
		 */
		filter_item->filter = event_filter(file);
		event_set_filter(file, filter);
		filter = NULL;

		fail = false;
	}

	if (fail)
		goto fail;

	/*
	 * The calls can still be using the old filters.
	 * Do a synchronize_rcu() and to ensure all calls are
	 * done with them before we free them.
	 */
	tracepoint_synchronize_unregister();
	list_for_each_entry_safe(filter_item, tmp, &filter_list, list) {
		__free_filter(filter_item->filter);
		list_del(&filter_item->list);
		kfree(filter_item);
	}
	return 0;
 fail:
	/* No call succeeded */
	list_for_each_entry_safe(filter_item, tmp, &filter_list, list) {
		list_del(&filter_item->list);
		kfree(filter_item);
	}
	parse_error(pe, FILT_ERR_BAD_SUBSYS_FILTER, 0);
	return -EINVAL;
 fail_mem:
	kfree(filter);
	/* If any call succeeded, we still need to sync */
	if (!fail)
		tracepoint_synchronize_unregister();
	list_for_each_entry_safe(filter_item, tmp, &filter_list, list) {
		__free_filter(filter_item->filter);
		list_del(&filter_item->list);
		kfree(filter_item);
	}
	return -ENOMEM;
}