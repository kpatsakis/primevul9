int filter_match_preds(struct event_filter *filter, void *rec)
{
	struct prog_entry *prog;
	int i;

	/* no filter is considered a match */
	if (!filter)
		return 1;

	/* Protected by either SRCU(tracepoint_srcu) or preempt_disable */
	prog = rcu_dereference_raw(filter->prog);
	if (!prog)
		return 1;

	for (i = 0; prog[i].pred; i++) {
		struct filter_pred *pred = prog[i].pred;
		int match = pred->fn(pred, rec);
		if (match == prog[i].when_to_branch)
			i = prog[i].target;
	}
	return prog[i].target;
}