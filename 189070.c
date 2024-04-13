regional_destroy(struct regional *r)
{
	if(!r) return;
	regional_free_all(r);
	free(r);
}