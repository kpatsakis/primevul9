regional_strdup(struct regional *r, const char *string)
{
	return (char*)regional_alloc_init(r, string, strlen(string)+1);
}