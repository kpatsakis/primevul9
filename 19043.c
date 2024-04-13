respip_tag_cfg(struct respip_set* set, const char* ipstr,
	const uint8_t* taglist, size_t taglen)
{
	struct resp_addr* node;

	if(!(node=respip_find_or_create(set, ipstr, 1)))
		return 0;
	if(node->taglist) {
		log_warn("duplicate response-address-tag for '%s', overridden.",
			ipstr);
	}
	node->taglist = regional_alloc_init(set->region, taglist, taglen);
	if(!node->taglist) {
		log_err("out of memory");
		return 0;
	}
	node->taglen = taglen;
	return 1;
}