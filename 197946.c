trace_free(void *arg, void *p_)
{
	trace_info *info = (trace_info *) arg;
	trace_header *p = (trace_header *)p_;

	if (p == NULL)
		return;
	info->current -= p[-1].size;
	free(&p[-1]);
}