static RzList *entries(RzBinFile *bf) {
	RzList *ret;
	RzBinAddr *ptr = NULL;
	QnxObj *qo = bf->o->bin_obj;
	if (!(ret = rz_list_new())) {
		return NULL;
	}
	ret->free = free;
	if (!(ptr = RZ_NEW0(RzBinAddr))) {
		return ret;
	}
	ptr->paddr = qo->lmfh.code_offset;
	ptr->vaddr = qo->lmfh.code_offset + baddr(bf);
	rz_list_append(ret, ptr);
	return ret;
}