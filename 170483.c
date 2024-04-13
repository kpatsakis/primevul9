RzList *MACH0_(get_virtual_files)(RzBinFile *bf) {
	rz_return_val_if_fail(bf, NULL);
	RzList *ret = rz_list_newf((RzListFree)rz_bin_virtual_file_free);
	if (!ret) {
		return NULL;
	}

	// rebasing+stripping for arm64e
	struct MACH0_(obj_t) *obj = bf->o->bin_obj;
	if (MACH0_(needs_rebasing_and_stripping)(obj)) {
		RzBinVirtualFile *vf = RZ_NEW0(RzBinVirtualFile);
		if (!vf) {
			return ret;
		}
		vf->buf = MACH0_(new_rebasing_and_stripping_buf)(obj);
		vf->buf_owned = true;
		vf->name = strdup(MACH0_VFILE_NAME_REBASED_STRIPPED);
		rz_list_push(ret, vf);
	}

	// clang-format off
	// relocs
	MACH0_(patch_relocs)(bf, obj);
	// clang-format: on
	// virtual file for reloc targets (where the relocs will point into)
	ut64 rtmsz = MACH0_(reloc_targets_vfile_size)(obj);
	if (rtmsz) {
		RzBuffer *buf = rz_buf_new_empty(rtmsz);
		if (!buf) {
			return ret;
		}
		RzBinVirtualFile *vf = RZ_NEW0(RzBinVirtualFile);
		if (!vf) {
			rz_buf_free(buf);
			return ret;
		}
		vf->buf = buf;
		vf->buf_owned = true;
		vf->name = strdup(MACH0_VFILE_NAME_RELOC_TARGETS);
		rz_list_push(ret, vf);
	}
	// virtual file mirroring the raw file, but with relocs patched
	if (obj->buf_patched) {
		RzBinVirtualFile *vf = RZ_NEW0(RzBinVirtualFile);
		if (!vf) {
			return ret;
		}
		vf->buf = obj->buf_patched;
		vf->buf_owned = false;
		vf->name = strdup(MACH0_VFILE_NAME_PATCHED);
		rz_list_push(ret, vf);
	}
	return ret;
}