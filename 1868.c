GF_Err gf_odf_delete_descriptor_list(GF_List *descList)
{
	GF_Err e;
	GF_Descriptor*tmp;
	u32 i;
	//no error if NULL chain...
	if (! descList) return GF_OK;
	i=0;
	while ((tmp = (GF_Descriptor*)gf_list_enum(descList, &i))) {
		e = gf_odf_delete_descriptor(tmp);
		if (e) return e;
	}
	gf_list_del(descList);
	return GF_OK;
}