R_API void r_core_anal_autoname_all_golang_fcns(RCore *core) {
	RList* section_list = r_bin_get_sections (core->bin);
	RListIter *iter;
	const char* oldstr = NULL;
	RBinSection *section;
	ut64 gopclntab = 0;
	r_list_foreach (section_list, iter, section) {
		if (strstr (section->name, ".gopclntab")) {
			gopclntab = section->vaddr;
			break;
		}
	}
	if (!gopclntab) {
		oldstr = r_print_rowlog (core->print, "Could not find .gopclntab section");
		r_print_rowlog_done (core->print, oldstr);
		return;
	}
	int ptr_size = core->anal->bits / 8;
	ut64 offset = gopclntab + 2 * ptr_size;
	ut64 size_offset = gopclntab + 3 * ptr_size ;
	ut8 temp_size[4] = {0};
	if (!r_io_nread_at (core->io, size_offset, temp_size, 4)) {
		return;
	}
	ut32 size = r_read_le32 (temp_size);
	int num_syms = 0;
	//r_cons_print ("[x] Reading .gopclntab...\n");
	r_flag_space_push (core->flags, R_FLAGS_FS_SYMBOLS);
	while (offset < gopclntab + size) {
		ut8 temp_delta[4] = {0};
		ut8 temp_func_addr[4] = {0};
		ut8 temp_func_name[4] = {0};
		if (!r_io_nread_at (core->io, offset + ptr_size, temp_delta, 4)) {
			break;
		}
		ut32 delta = r_read_le32 (temp_delta);
		ut64 func_offset = gopclntab + delta;
		if (!r_io_nread_at (core->io, func_offset, temp_func_addr, 4) ||
			!r_io_nread_at (core->io, func_offset + ptr_size, temp_func_name, 4)) {
			break;
		}
		ut32 func_addr = r_read_le32 (temp_func_addr);
		ut32 func_name_offset = r_read_le32 (temp_func_name);
		ut8 func_name[64] = {0};
		r_io_read_at (core->io, gopclntab + func_name_offset, func_name, 63);
		if (func_name[0] == 0xff) {
			break;
		}
		r_name_filter ((char *)func_name, 0);
		//r_cons_printf ("[x] Found symbol %s at 0x%x\n", func_name, func_addr);
		char *flagname = r_str_newf ("sym.go.%s", func_name);
		r_flag_set (core->flags, flagname, func_addr, 1);
		free (flagname);
		offset += 2 * ptr_size;
		num_syms++;
	}
	r_flag_space_pop (core->flags);
	if (num_syms) {
		r_strf_var (msg, 128, "Found %d symbols and saved them at sym.go.*", num_syms);
		oldstr = r_print_rowlog (core->print, msg);
		r_print_rowlog_done (core->print, oldstr);
	} else {
		oldstr = r_print_rowlog (core->print, "Found no symbols.");
		r_print_rowlog_done (core->print, oldstr);
	}
}