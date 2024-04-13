GF_Err gf_odf_write_descriptor_list(GF_BitStream *bs, GF_List *descList)
{
	GF_Err e;
	u32 count, i;

	if (! descList) return GF_OK;
	count = gf_list_count(descList);
	for ( i = 0; i < count; i++ ) {
		GF_Descriptor *tmp = (GF_Descriptor*)gf_list_get(descList, i);
		if (tmp) {
			e = gf_odf_write_descriptor(bs, tmp);
			if (e) return e;
		}
	}
	return GF_OK;
}