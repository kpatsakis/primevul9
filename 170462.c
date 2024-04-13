struct import_t *MACH0_(get_imports)(struct MACH0_(obj_t) * bin) {
	rz_return_val_if_fail(bin, NULL);

	int i, j, idx, stridx;
	if (!bin->sects || !bin->symtab || !bin->symstr || !bin->indirectsyms) {
		return NULL;
	}

	if (bin->dysymtab.nundefsym < 1 || bin->dysymtab.nundefsym > 0xfffff) {
		return NULL;
	}

	struct import_t *imports = calloc(bin->dysymtab.nundefsym + 1, sizeof(struct import_t));
	if (!imports) {
		return NULL;
	}
	for (i = j = 0; i < bin->dysymtab.nundefsym; i++) {
		idx = bin->dysymtab.iundefsym + i;
		if (idx < 0 || idx >= bin->nsymtab) {
			bprintf("WARNING: Imports index out of bounds. Ignoring relocs\n");
			free(imports);
			return NULL;
		}
		stridx = bin->symtab[idx].n_strx;
		char *imp_name = MACH0_(get_name)(bin, stridx, false);
		if (imp_name) {
			rz_str_ncpy(imports[j].name, imp_name, RZ_BIN_MACH0_STRING_LENGTH);
			free(imp_name);
		} else {
			// imports[j].name[0] = 0;
			continue;
		}
		imports[j].ord = i;
		imports[j++].last = 0;
	}
	imports[j].last = 1;

	if (!bin->imports_by_ord_size) {
		if (j > 0) {
			bin->imports_by_ord_size = j;
			bin->imports_by_ord = (RzBinImport **)calloc(j, sizeof(RzBinImport *));
		} else {
			bin->imports_by_ord_size = 0;
			bin->imports_by_ord = NULL;
		}
	}

	return imports;
}