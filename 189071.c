count_chunks(struct regional* r)
{
	size_t c = 1;
	char* p = r->next;
	while(p) {
		c++;
		p = *(char**)p;
	}
	return c;
}