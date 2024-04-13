const struct symbol_t *MACH0_(get_symbols)(struct MACH0_(obj_t) * bin) {
	struct symbol_t *symbols;
	int j = 0, s = 0, stridx = 0;
	size_t symbols_size = 0, symbols_count = 0;
	ut64 to = 0, from = 0, i = 0;

	if (bin->symbols) {
		return bin->symbols;
	}

	HtPP *hash = ht_pp_new0();
	if (!hash) {
		return NULL;
	}

	rz_return_val_if_fail(bin, NULL);
	int n_exports = walk_exports(bin, NULL, NULL);

	symbols_count = n_exports;
	j = 0; // symbol_idx

	int bits = MACH0_(get_bits_from_hdr)(&bin->hdr);
	if (bin->symtab && bin->symstr) {
		/* parse dynamic symbol table */
		symbols_count = (bin->dysymtab.nextdefsym +
			bin->dysymtab.nlocalsym +
			bin->dysymtab.nundefsym);
		symbols_count += (bin->nsymtab + 1);
		if (SZT_MUL_OVFCHK(symbols_count, 2)) {
			RZ_LOG_ERROR("mach0: detected symbols count overflow\n");
			ht_pp_free(hash);
			return NULL;
		}
		symbols_size = symbols_count * 2;
		symbols = RZ_NEWS0(struct symbol_t, symbols_size);
		if (!symbols) {
			ht_pp_free(hash);
			return NULL;
		}
		bin->main_addr = 0;
		for (s = 0; s < 2; s++) {
			switch (s) {
			case 0:
				from = bin->dysymtab.iextdefsym;
				to = from + bin->dysymtab.nextdefsym;
				break;
			case 1:
				from = bin->dysymtab.ilocalsym;
				to = from + bin->dysymtab.nlocalsym;
				break;
#if NOT_USED
			case 2:
				from = bin->dysymtab.iundefsym;
				to = from + bin->dysymtab.nundefsym;
				break;
#endif
			}
			if (from == to) {
				continue;
			}

			from = RZ_MIN(RZ_MAX(0, from), symbols_size);
			to = RZ_MIN(RZ_MIN(to, bin->nsymtab), symbols_size);

			ut32 maxsymbols = symbols_size;
			if (symbols_count >= maxsymbols) {
				symbols_count = maxsymbols - 1;
				RZ_LOG_WARN("mach0: symbol table is truncated\n");
			}
			for (i = from; i < to && j < symbols_count; i++, j++) {
				symbols[j].offset = MACH0_(vaddr_to_paddr)(bin, bin->symtab[i].n_value);
				symbols[j].addr = bin->symtab[i].n_value;
				symbols[j].size = 0; /* TODO: Is it anywhere? */
				symbols[j].bits = bin->symtab[i].n_desc & N_ARM_THUMB_DEF ? 16 : bits;
				symbols[j].is_imported = false;
				symbols[j].type = (bin->symtab[i].n_type & N_EXT)
					? RZ_BIN_MACH0_SYMBOL_TYPE_EXT
					: RZ_BIN_MACH0_SYMBOL_TYPE_LOCAL;
				stridx = bin->symtab[i].n_strx;
				symbols[j].name = MACH0_(get_name)(bin, stridx, false);
				symbols[j].last = false;

				const char *name = symbols[j].name;
				if (bin->main_addr == 0 && name) {
					if (!strcmp(name, "__Dmain")) {
						bin->main_addr = symbols[j].addr;
					} else if (strstr(name, "4main") && !strstr(name, "STATIC")) {
						bin->main_addr = symbols[j].addr;
					} else if (!strcmp(name, "_main")) {
						bin->main_addr = symbols[j].addr;
					} else if (!strcmp(name, "main")) {
						bin->main_addr = symbols[j].addr;
					}
				}
				if (inSymtab(hash, symbols[j].name, symbols[j].addr)) {
					free(symbols[j].name);
					symbols[j].name = NULL;
					j--;
				}
			}
		}
		to = RZ_MIN((ut32)bin->nsymtab, bin->dysymtab.iundefsym + bin->dysymtab.nundefsym);
		for (i = bin->dysymtab.iundefsym; i < to; i++) {
			if (j > symbols_count) {
				bprintf("mach0-get-symbols: error\n");
				break;
			}
			if (parse_import_stub(bin, &symbols[j], i)) {
				symbols[j++].last = false;
			}
		}

		for (i = 0; i < bin->nsymtab && i < symbols_count; i++) {
			struct MACH0_(nlist) *st = &bin->symtab[i];
			if (st->n_type & N_STAB) {
				continue;
			}
			// 0 is for imports
			// 1 is for symbols
			// 2 is for func.eh (exception handlers?)
			int section = st->n_sect;
			if (section == 1 && j < symbols_count) {
				// check if symbol exists already
				/* is symbol */
				symbols[j].addr = st->n_value;
				symbols[j].offset = MACH0_(vaddr_to_paddr)(bin, symbols[j].addr);
				symbols[j].size = 0; /* find next symbol and crop */
				symbols[j].type = (st->n_type & N_EXT)
					? RZ_BIN_MACH0_SYMBOL_TYPE_EXT
					: RZ_BIN_MACH0_SYMBOL_TYPE_LOCAL;
				char *sym_name = MACH0_(get_name)(bin, st->n_strx, false);
				if (sym_name) {
					symbols[j].name = sym_name;
				} else {
					symbols[j].name = rz_str_newf("entry%" PFMT64u, i);
				}
				symbols[j].last = false;
				if (inSymtab(hash, symbols[j].name, symbols[j].addr)) {
					RZ_FREE(symbols[j].name);
				} else {
					j++;
				}

				const char *name = symbols[i].name;
				if (bin->main_addr == 0 && name) {
					if (name && !strcmp(name, "__Dmain")) {
						bin->main_addr = symbols[i].addr;
					} else if (name && strstr(name, "4main") && !strstr(name, "STATIC")) {
						bin->main_addr = symbols[i].addr;
					} else if (symbols[i].name && !strcmp(symbols[i].name, "_main")) {
						bin->main_addr = symbols[i].addr;
					}
				}
			}
		}
	} else if (!n_exports) {
		ht_pp_free(hash);
		return NULL;
	} else {
		if (SZT_ADD_OVFCHK(symbols_count, 1)) {
			ht_pp_free(hash);
			return NULL;
		}
		symbols_size = symbols_count + 1;
		if (!(symbols = RZ_NEWS0(struct symbol_t, symbols_size))) {
			ht_pp_free(hash);
			return NULL;
		}
	}
	if (n_exports && (symbols_count - j) >= n_exports) {
		RSymCtx sym_ctx;
		sym_ctx.symbols = symbols;
		sym_ctx.j = j;
		sym_ctx.symbols_count = symbols_count;
		sym_ctx.hash = hash;
		walk_exports(bin, assign_export_symbol_t, &sym_ctx);
		j = sym_ctx.j;
	}
	ht_pp_free(hash);
	symbols[j].last = true;
	bin->symbols = symbols;
	return symbols;
}