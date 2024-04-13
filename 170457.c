ut64 MACH0_(get_main)(struct MACH0_(obj_t) * bin) {
	ut64 addr = UT64_MAX;
	int i;

	// 0 = sscanned but no main found
	// -1 = not scanned, so no main
	// other = valid main addr
	if (bin->main_addr == UT64_MAX) {
		(void)MACH0_(get_symbols)(bin);
	}
	if (bin->main_addr != 0 && bin->main_addr != UT64_MAX) {
		return bin->main_addr;
	}
	// dummy call to initialize things
	free(MACH0_(get_entrypoint)(bin));

	bin->main_addr = 0;

	if (addr == UT64_MAX && bin->main_cmd.cmd == LC_MAIN) {
		addr = bin->entry + bin->baddr;
	}

	if (!addr) {
		ut8 b[128];
		ut64 entry = MACH0_(vaddr_to_paddr)(bin, bin->entry);
		// XXX: X86 only and hacky!
		if (entry > bin->size || entry + sizeof(b) > bin->size) {
			return UT64_MAX;
		}
		i = rz_buf_read_at(bin->b, entry, b, sizeof(b));
		if (i < 80) {
			return UT64_MAX;
		}
		for (i = 0; i < 64; i++) {
			if (b[i] == 0xe8 && !b[i + 3] && !b[i + 4]) {
				int delta = b[i + 1] | (b[i + 2] << 8) | (b[i + 3] << 16) | (b[i + 4] << 24);
				addr = bin->entry + i + 5 + delta;
				break;
			}
		}
		if (!addr) {
			addr = entry;
		}
	}
	return bin->main_addr = addr;
}