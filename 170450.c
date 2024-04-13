static bool parse_import_stub(struct MACH0_(obj_t) * bin, struct symbol_t *symbol, int idx) {
	size_t i, j, nsyms, stridx;
	const char *symstr;
	if (idx < 0) {
		return false;
	}
	symbol->offset = 0LL;
	symbol->addr = 0LL;
	symbol->name = NULL;
	symbol->is_imported = true;

	if (!bin || !bin->sects) {
		return false;
	}
	for (i = 0; i < bin->nsects; i++) {
		if ((bin->sects[i].flags & SECTION_TYPE) == S_SYMBOL_STUBS && bin->sects[i].reserved2 > 0) {
			ut64 sect_size = bin->sects[i].size;
			ut32 sect_fragment = bin->sects[i].reserved2;
			if (bin->sects[i].offset > bin->size) {
				bprintf("mach0: section offset starts way beyond the end of the file\n");
				continue;
			}
			if (sect_size > bin->size) {
				bprintf("mach0: Invalid symbol table size\n");
				sect_size = bin->size - bin->sects[i].offset;
			}
			nsyms = (int)(sect_size / sect_fragment);
			for (j = 0; j < nsyms; j++) {
				if (bin->sects) {
					if (bin->sects[i].reserved1 + j >= bin->nindirectsyms) {
						continue;
					}
				}
				if (bin->indirectsyms) {
					if (idx != bin->indirectsyms[bin->sects[i].reserved1 + j]) {
						continue;
					}
				}
				if (idx > bin->nsymtab) {
					continue;
				}
				symbol->type = RZ_BIN_MACH0_SYMBOL_TYPE_LOCAL;
				int delta = j * bin->sects[i].reserved2;
				if (delta < 0) {
					bprintf("mach0: corrupted reserved2 value leads to int overflow.\n");
					continue;
				}
				symbol->offset = bin->sects[i].offset + delta;
				symbol->addr = bin->sects[i].addr + delta;
				symbol->size = 0;
				stridx = bin->symtab[idx].n_strx;
				if (stridx < bin->symstrlen) {
					symstr = (char *)bin->symstr + stridx;
				} else {
					symstr = "???";
				}
				// Remove the extra underscore that every import seems to have in Mach-O.
				if (*symstr == '_') {
					symstr++;
				}
				symbol->name = strdup(symstr);
				return true;
			}
		}
	}
	return false;
}