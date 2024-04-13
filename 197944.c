trace_malloc(void *arg, size_t size)
{
	trace_info *info = (trace_info *) arg;
	trace_header *p;
	if (size == 0)
		return NULL;
	p = malloc(size + sizeof(trace_header));
	if (p == NULL)
		return NULL;
	p[0].size = size;
	info->current += size;
	info->total += size;
	if (info->current > info->peak)
		info->peak = info->current;
	return (void *)&p[1];
}