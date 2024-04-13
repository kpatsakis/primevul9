static int create_system_filter(struct trace_subsystem_dir *dir,
				struct trace_array *tr,
				char *filter_str, struct event_filter **filterp)
{
	struct filter_parse_error *pe = NULL;
	int err;

	err = create_filter_start(filter_str, true, &pe, filterp);
	if (!err) {
		err = process_system_preds(dir, tr, pe, filter_str);
		if (!err) {
			/* System filters just show a default message */
			kfree((*filterp)->filter_string);
			(*filterp)->filter_string = NULL;
		} else {
			append_filter_err(tr, pe, *filterp);
		}
	}
	create_filter_finish(pe);

	return err;
}