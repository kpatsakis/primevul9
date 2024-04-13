RzList *MACH0_(mach_fields)(RzBinFile *bf) {
	RzBuffer *buf = bf->buf;
	ut64 length = rz_buf_size(buf);
	struct MACH0_(mach_header) *mh = MACH0_(get_hdr)(buf);
	if (!mh) {
		return NULL;
	}
	RzList *ret = rz_list_newf((RzListFree)rz_bin_field_free);
	if (!ret) {
		free(mh);
		return NULL;
	}
	ut64 addr = pa2va(bf, 0);
	ut64 paddr = 0;

	rz_list_append(ret, rz_bin_field_new(addr, addr, 1, "header", "mach0_header", "mach0_header", true));
	addr += 0x20 - 4;
	paddr += 0x20 - 4;
	bool is64 = mh->cputype >> 16;
	if (is64) {
		addr += 4;
		paddr += 4;
	}

	bool isBe = false;
	switch (mh->cputype) {
	case CPU_TYPE_POWERPC:
	case CPU_TYPE_POWERPC64:
		isBe = true;
		break;
	}

	int n;
	for (n = 0; n < mh->ncmds; n++) {
		ut32 lcType;
		if (!rz_buf_read_ble32_at(buf, paddr, &lcType, isBe)) {
			break;
		}
		ut32 word;
		if (!rz_buf_read_ble32_at(buf, paddr + 4, &word, isBe)) {
			break;
		}
		if (paddr + 8 > length) {
			break;
		}
		ut32 lcSize = word;
		word &= 0xFFFFFF;
		if (lcSize < 1) {
			eprintf("Invalid size for a load command\n");
			break;
		}
		if (word == 0) {
			break;
		}
		const char *pf_definition = cmd_to_pf_definition(lcType);
		if (pf_definition) {
			rz_list_append(ret, rz_bin_field_new(addr, addr, 1, sdb_fmt("load_command_%d_%s", n, cmd_to_string(lcType)), pf_definition, pf_definition, true));
		}
		switch (lcType) {
		case LC_BUILD_VERSION: {
			ut32 ntools;
			if (!rz_buf_read_le32_at(buf, paddr + 20, &ntools)) {
				break;
			}
			ut64 off = 24;
			int j = 0;
			while (off < lcSize && ntools--) {
				rz_list_append(ret, rz_bin_field_new(addr + off, addr + off, 1, sdb_fmt("tool_%d", j++), "mach0_build_version_tool", "mach0_build_version_tool", true));
				off += 8;
			}
			break;
		}
		case LC_SEGMENT:
		case LC_SEGMENT_64: {
			ut32 nsects;
			if (!rz_buf_read_le32_at(buf, addr + (is64 ? 64 : 48), &nsects)) {
				break;
			}
			ut64 off = is64 ? 72 : 56;
			size_t i, j = 0;
			for (i = 0; i < nsects && (addr + off) < length && off < lcSize; i++) {
				const char *sname = is64 ? "mach0_section64" : "mach0_section";
				RzBinField *f = rz_bin_field_new(addr + off, addr + off, 1,
					sdb_fmt("section_%zu", j++), sname, sname, true);
				rz_list_append(ret, f);
				off += is64 ? 80 : 68;
			}
			break;
		default:
			// TODO
			break;
		}
		}
		addr += word;
		paddr += word;
	}
	free(mh);
	return ret;
}