RZ_API char *rz_bin_dwarf_line_header_get_full_file_path(RZ_NULLABLE const RzBinDwarfDebugInfo *info, const RzBinDwarfLineHeader *header, ut64 file_index) {
	rz_return_val_if_fail(header, NULL);
	if (file_index >= header->file_names_count) {
		return NULL;
	}
	RzBinDwarfLineFileEntry *file = &header->file_names[file_index];
	if (!file->name) {
		return NULL;
	}

	/*
	 * Dwarf standard does not seem to specify the exact separator (slash/backslash) of paths
	 * so apparently it is target-dependent. However we have yet to see a Windows binary that
	 * also contains dwarf and contains backslashes. The ones we have seen from MinGW have regular
	 * slashes.
	 * And since there seems to be no way to reliable check whether the target uses slashes
	 * or backslashes anyway, we will simply use slashes always here.
	 */

	const char *comp_dir = info ? ht_up_find(info->line_info_offset_comp_dir, header->offset, NULL) : NULL;
	const char *include_dir = NULL;
	char *own_str = NULL;
	if (file->id_idx > 0 && file->id_idx - 1 < header->include_dirs_count) {
		include_dir = header->include_dirs[file->id_idx - 1];
		if (include_dir && include_dir[0] != '/' && comp_dir) {
			include_dir = own_str = rz_str_newf("%s/%s/", comp_dir, include_dir);
		}
	} else {
		include_dir = comp_dir;
	}
	if (!include_dir) {
		include_dir = "./";
	}
	char *r = rz_str_newf("%s/%s", include_dir, file->name);
	free(own_str);
	return r;
}