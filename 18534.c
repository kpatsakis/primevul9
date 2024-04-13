cmpstringp(const void *p1, const void *p2)
{
	return strcmp(*(char * const *)p1, *(char * const *)p2);
}