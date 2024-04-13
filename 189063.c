count_large(struct regional* r)
{
	size_t c = 0;
	char* p = r->large_list;
	while(p) {
		c++;
		p = *(char**)p;
	}
	return c;
}