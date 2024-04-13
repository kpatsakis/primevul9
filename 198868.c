wc_ucs_get_tag(int ntag)
{
    if (ntag == 0 || ntag > n_tag_map)
	return NULL;
    return tag_map[ntag];
}