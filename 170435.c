struct addr_t *MACH0_(get_entrypoint)(struct MACH0_(obj_t) * bin) {
	rz_return_val_if_fail(bin, NULL);

	ut64 ea = entry_to_vaddr(bin);
	if (ea == 0 || ea == UT64_MAX) {
		return NULL;
	}
	struct addr_t *entry = RZ_NEW0(struct addr_t);
	if (!entry) {
		return NULL;
	}
	entry->addr = ea;
	entry->offset = MACH0_(vaddr_to_paddr)(bin, entry->addr);
	entry->haddr = sdb_num_get(bin->kv, "mach0.entry.offset", 0);
	sdb_num_set(bin->kv, "mach0.entry.vaddr", entry->addr, 0);
	sdb_num_set(bin->kv, "mach0.entry.paddr", bin->entry, 0);

	if (entry->offset == 0 && !bin->sects) {
		int i;
		for (i = 0; i < bin->nsects; i++) {
			// XXX: section name shoudnt matter .. just check for exec flags
			if (!strncmp(bin->sects[i].sectname, "__text", 6)) {
				entry->offset = (ut64)bin->sects[i].offset;
				sdb_num_set(bin->kv, "mach0.entry", entry->offset, 0);
				entry->addr = (ut64)bin->sects[i].addr;
				if (!entry->addr) { // workaround for object files
					eprintf("entrypoint is 0...\n");
					// XXX(lowlyw) there's technically not really entrypoints
					// for .o files, so ignore this...
					// entry->addr = entry->offset;
				}
				break;
			}
		}
		bin->entry = entry->addr;
	}
	return entry;
}