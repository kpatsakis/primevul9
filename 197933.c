trace_realloc(void *arg, void *p_, size_t size)
{
	trace_info *info = (trace_info *) arg;
	trace_header *p = (trace_header *)p_;
	size_t oldsize;

	if (size == 0)
	{
		trace_free(arg, p_);
		return NULL;
	}
	if (p == NULL)
		return trace_malloc(arg, size);
	oldsize = p[-1].size;
	p = realloc(&p[-1], size + sizeof(trace_header));
	if (p == NULL)
		return NULL;
	info->current += size - oldsize;
	if (size > oldsize)
		info->total += size - oldsize;
	if (info->current > info->peak)
		info->peak = info->current;
	p[0].size = size;
	return &p[1];
}