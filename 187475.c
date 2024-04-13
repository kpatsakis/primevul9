static void *data_block(struct mdesc_hdr *mdesc)
{
	return ((void *) name_block(mdesc)) + mdesc->name_sz;
}