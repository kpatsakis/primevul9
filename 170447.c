static bool parse_dysymtab(struct MACH0_(obj_t) * bin, ut64 off) {
	size_t len, i;
	ut32 size_tab;
	ut8 dysym[sizeof(struct dysymtab_command)] = { 0 };
	ut8 dytoc[sizeof(struct dylib_table_of_contents)] = { 0 };
	ut8 dymod[sizeof(struct MACH0_(dylib_module))] = { 0 };
	ut8 idsyms[sizeof(ut32)] = { 0 };

	if (off > bin->size || off + sizeof(struct dysymtab_command) > bin->size) {
		return false;
	}

	len = rz_buf_read_at(bin->b, off, dysym, sizeof(struct dysymtab_command));
	if (len != sizeof(struct dysymtab_command)) {
		bprintf("Error: read (dysymtab)\n");
		return false;
	}

	bin->dysymtab.cmd = rz_read_ble32(&dysym[0], bin->big_endian);
	bin->dysymtab.cmdsize = rz_read_ble32(&dysym[4], bin->big_endian);
	bin->dysymtab.ilocalsym = rz_read_ble32(&dysym[8], bin->big_endian);
	bin->dysymtab.nlocalsym = rz_read_ble32(&dysym[12], bin->big_endian);
	bin->dysymtab.iextdefsym = rz_read_ble32(&dysym[16], bin->big_endian);
	bin->dysymtab.nextdefsym = rz_read_ble32(&dysym[20], bin->big_endian);
	bin->dysymtab.iundefsym = rz_read_ble32(&dysym[24], bin->big_endian);
	bin->dysymtab.nundefsym = rz_read_ble32(&dysym[28], bin->big_endian);
	bin->dysymtab.tocoff = rz_read_ble32(&dysym[32], bin->big_endian);
	bin->dysymtab.ntoc = rz_read_ble32(&dysym[36], bin->big_endian);
	bin->dysymtab.modtaboff = rz_read_ble32(&dysym[40], bin->big_endian);
	bin->dysymtab.nmodtab = rz_read_ble32(&dysym[44], bin->big_endian);
	bin->dysymtab.extrefsymoff = rz_read_ble32(&dysym[48], bin->big_endian);
	bin->dysymtab.nextrefsyms = rz_read_ble32(&dysym[52], bin->big_endian);
	bin->dysymtab.indirectsymoff = rz_read_ble32(&dysym[56], bin->big_endian);
	bin->dysymtab.nindirectsyms = rz_read_ble32(&dysym[60], bin->big_endian);
	bin->dysymtab.extreloff = rz_read_ble32(&dysym[64], bin->big_endian);
	bin->dysymtab.nextrel = rz_read_ble32(&dysym[68], bin->big_endian);
	bin->dysymtab.locreloff = rz_read_ble32(&dysym[72], bin->big_endian);
	bin->dysymtab.nlocrel = rz_read_ble32(&dysym[76], bin->big_endian);

	bin->ntoc = bin->dysymtab.ntoc;
	if (bin->ntoc > 0) {
		if (!(bin->toc = calloc(bin->ntoc, sizeof(struct dylib_table_of_contents)))) {
			perror("calloc (toc)");
			return false;
		}
		if (!UT32_MUL(&size_tab, bin->ntoc, sizeof(struct dylib_table_of_contents))) {
			RZ_FREE(bin->toc);
			return false;
		}
		if (!size_tab) {
			RZ_FREE(bin->toc);
			return false;
		}
		if (bin->dysymtab.tocoff > bin->size || bin->dysymtab.tocoff + size_tab > bin->size) {
			RZ_FREE(bin->toc);
			return false;
		}
		for (i = 0; i < bin->ntoc; i++) {
			len = rz_buf_read_at(bin->b, bin->dysymtab.tocoff + i * sizeof(struct dylib_table_of_contents),
				dytoc, sizeof(struct dylib_table_of_contents));
			if (len != sizeof(struct dylib_table_of_contents)) {
				bprintf("Error: read (toc)\n");
				RZ_FREE(bin->toc);
				return false;
			}
			bin->toc[i].symbol_index = rz_read_ble32(&dytoc[0], bin->big_endian);
			bin->toc[i].module_index = rz_read_ble32(&dytoc[4], bin->big_endian);
		}
	}
	bin->nmodtab = bin->dysymtab.nmodtab;
	if (bin->nmodtab > 0) {
		if (!(bin->modtab = calloc(bin->nmodtab, sizeof(struct MACH0_(dylib_module))))) {
			perror("calloc (modtab)");
			return false;
		}
		if (!UT32_MUL(&size_tab, bin->nmodtab, sizeof(struct MACH0_(dylib_module)))) {
			RZ_FREE(bin->modtab);
			return false;
		}
		if (!size_tab) {
			RZ_FREE(bin->modtab);
			return false;
		}
		if (bin->dysymtab.modtaboff > bin->size ||
			bin->dysymtab.modtaboff + size_tab > bin->size) {
			RZ_FREE(bin->modtab);
			return false;
		}
		for (i = 0; i < bin->nmodtab; i++) {
			len = rz_buf_read_at(bin->b, bin->dysymtab.modtaboff + i * sizeof(struct MACH0_(dylib_module)),
				dymod, sizeof(struct MACH0_(dylib_module)));
			if (len == -1) {
				bprintf("Error: read (modtab)\n");
				RZ_FREE(bin->modtab);
				return false;
			}

			bin->modtab[i].module_name = rz_read_ble32(&dymod[0], bin->big_endian);
			bin->modtab[i].iextdefsym = rz_read_ble32(&dymod[4], bin->big_endian);
			bin->modtab[i].nextdefsym = rz_read_ble32(&dymod[8], bin->big_endian);
			bin->modtab[i].irefsym = rz_read_ble32(&dymod[12], bin->big_endian);
			bin->modtab[i].nrefsym = rz_read_ble32(&dymod[16], bin->big_endian);
			bin->modtab[i].ilocalsym = rz_read_ble32(&dymod[20], bin->big_endian);
			bin->modtab[i].nlocalsym = rz_read_ble32(&dymod[24], bin->big_endian);
			bin->modtab[i].iextrel = rz_read_ble32(&dymod[28], bin->big_endian);
			bin->modtab[i].nextrel = rz_read_ble32(&dymod[32], bin->big_endian);
			bin->modtab[i].iinit_iterm = rz_read_ble32(&dymod[36], bin->big_endian);
			bin->modtab[i].ninit_nterm = rz_read_ble32(&dymod[40], bin->big_endian);
#if RZ_BIN_MACH064
			bin->modtab[i].objc_module_info_size = rz_read_ble32(&dymod[44], bin->big_endian);
			bin->modtab[i].objc_module_info_addr = rz_read_ble64(&dymod[48], bin->big_endian);
#else
			bin->modtab[i].objc_module_info_addr = rz_read_ble32(&dymod[44], bin->big_endian);
			bin->modtab[i].objc_module_info_size = rz_read_ble32(&dymod[48], bin->big_endian);
#endif
		}
	}
	bin->nindirectsyms = bin->dysymtab.nindirectsyms;
	if (bin->nindirectsyms > 0) {
		if (!(bin->indirectsyms = calloc(bin->nindirectsyms, sizeof(ut32)))) {
			perror("calloc (indirectsyms)");
			return false;
		}
		if (!UT32_MUL(&size_tab, bin->nindirectsyms, sizeof(ut32))) {
			RZ_FREE(bin->indirectsyms);
			return false;
		}
		if (!size_tab) {
			RZ_FREE(bin->indirectsyms);
			return false;
		}
		if (bin->dysymtab.indirectsymoff > bin->size ||
			bin->dysymtab.indirectsymoff + size_tab > bin->size) {
			RZ_FREE(bin->indirectsyms);
			return false;
		}
		for (i = 0; i < bin->nindirectsyms; i++) {
			len = rz_buf_read_at(bin->b, bin->dysymtab.indirectsymoff + i * sizeof(ut32), idsyms, 4);
			if (len == -1) {
				bprintf("Error: read (indirect syms)\n");
				RZ_FREE(bin->indirectsyms);
				return false;
			}
			bin->indirectsyms[i] = rz_read_ble32(&idsyms[0], bin->big_endian);
		}
	}
	/* TODO extrefsyms, extrel, locrel */
	return true;
}