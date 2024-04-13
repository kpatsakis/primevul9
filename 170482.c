RzList *MACH0_(get_maps_unpatched)(RzBinFile *bf) {
	rz_return_val_if_fail(bf, NULL);
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	RzList *ret = rz_list_newf((RzListFree)rz_bin_map_free);
	if (!ret) {
		return NULL;
	}
	for (size_t i = 0; i < bin->nsegs; i++) {
		struct MACH0_(segment_command) *seg = &bin->segs[i];
		if (!seg->initprot) {
			continue;
		}
		RzBinMap *map = RZ_NEW0(RzBinMap);
		if (!map) {
			break;
		}
		map->psize = seg->vmsize;
		map->vaddr = seg->vmaddr;
		map->vsize = seg->vmsize;
		map->name = rz_str_ndup(seg->segname, 16);
		rz_str_filter(map->name);
		map->perm = prot2perm(seg->initprot);
		if (MACH0_(segment_needs_rebasing_and_stripping)(bin, i)) {
			map->vfile_name = strdup(MACH0_VFILE_NAME_REBASED_STRIPPED);
			map->paddr = seg->fileoff;
		} else {
			// boffset is relevant for fatmach0 where the mach0 is located boffset into the whole file
			// the rebasing vfile above however is based at the mach0 already
			map->paddr = seg->fileoff + bf->o->boffset;
		}
		rz_list_append(ret, map);
	}
	return ret;
}