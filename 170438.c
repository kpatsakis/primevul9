RzList *MACH0_(get_maps)(RzBinFile *bf) {
	RzList *ret = MACH0_(get_maps_unpatched)(bf);
	if (!ret) {
		return NULL;
	}
	struct MACH0_(obj_t) *obj = bf->o->bin_obj;
	// clang-format off
	MACH0_(patch_relocs)(bf, obj);
	// clang-format on
	rz_bin_relocs_patch_maps(ret, obj->buf_patched, bf->o->boffset,
		MACH0_(reloc_targets_map_base)(bf, obj), MACH0_(reloc_targets_vfile_size)(obj),
		MACH0_VFILE_NAME_PATCHED, MACH0_VFILE_NAME_RELOC_TARGETS);
	return ret;
}