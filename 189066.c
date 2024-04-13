regional_free_all(struct regional *r)
{
	char* p = r->next, *np;
	while(p) {
		np = *(char**)p;
		free(p);
		p = np;
	}
	p = r->large_list;
	while(p) {
		np = *(char**)p;
		free(p);
		p = np;
	}
	regional_init(r);
}