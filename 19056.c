static RzList *relocs(RzBinFile *bf) {
	rz_return_val_if_fail(bf && bf->o, NULL);
	QnxObj *qo = bf->o->bin_obj;
	RzBinReloc *reloc = NULL;
	RzListIter *it = NULL;
	RzList *relocs = rz_list_newf(free);
	if (!relocs) {
		return NULL;
	}

	rz_list_foreach (qo->fixups, it, reloc) {
		RzBinReloc *copy = RZ_NEW0(RzBinReloc);
		if (!copy) {
			break;
		}
		copy->vaddr = reloc->vaddr;
		copy->paddr = reloc->paddr;
		copy->type = reloc->type;
		rz_list_append(relocs, copy);
	}
	return relocs;
}