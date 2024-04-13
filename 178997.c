static void *atalk_seq_route_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct atalk_route *r;

	++*pos;
	if (v == SEQ_START_TOKEN) {
		r = NULL;
		if (atalk_routes)
			r = atalk_routes;
		goto out;
	}
	r = v;
	r = r->next;
out:
	return r;
}