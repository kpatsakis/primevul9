wc_ucs_put_tag(char *p)
{
    int i;

    if (p == NULL || *p == '\0')
	return 0;
    for (i = 1; i <= n_tag_map; i++) {
	if (!strcasecmp(p, tag_map[i]))
	    return i;
    }
    n_tag_map++;
    if (n_tag_map == MAX_TAG_MAP)
	return 0;
    tag_map[n_tag_map] = p;
    return n_tag_map;
}