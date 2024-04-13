static void assign_export_symbol_t(struct MACH0_(obj_t) * bin, const char *name, ut64 flags, ut64 offset, void *ctx) {
	RSymCtx *sym_ctx = (RSymCtx *)ctx;
	int j = sym_ctx->j;
	if (j < sym_ctx->symbols_count) {
		sym_ctx->symbols[j].offset = offset;
		sym_ctx->symbols[j].addr = MACH0_(paddr_to_vaddr)(bin, offset);
		if (inSymtab(sym_ctx->hash, name, sym_ctx->symbols[j].addr)) {
			return;
		}
		sym_ctx->symbols[j].size = 0;
		sym_ctx->symbols[j].type = RZ_BIN_MACH0_SYMBOL_TYPE_EXT;
		sym_ctx->symbols[j].name = strdup(name);
		sym_ctx->j++;
	}
}