void MACH0_(mach_headerfields)(RBinFile *bf) {
	PrintfCallback cb_printf = bf->rbin->cb_printf;
	if (!cb_printf) {
		cb_printf = printf;
	}
	RBuffer *buf = bf->buf;
	ut64 length = r_buf_size (buf);
	int n = 0;
	struct MACH0_(mach_header) *mh = MACH0_(get_hdr)(buf);
	if (!mh) {
		return;
	}
	ut64 pvaddr = pa2va (bf, 0);
	cb_printf ("pf.mach0_header @ 0x%08"PFMT64x"\n", pvaddr);
	cb_printf ("0x%08"PFMT64x"  Magic       0x%x\n", pvaddr, mh->magic);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  CpuType     0x%x\n", pvaddr, mh->cputype);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  CpuSubType  0x%x\n", pvaddr, mh->cpusubtype);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  FileType    0x%x\n", pvaddr, mh->filetype);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  nCmds       %d\n", pvaddr, mh->ncmds);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  sizeOfCmds  %d\n", pvaddr, mh->sizeofcmds);
	pvaddr += 4;
	cb_printf ("0x%08"PFMT64x"  Flags       0x%x\n", pvaddr, mh->flags);
	pvaddr += 4;
	bool is64 = mh->cputype >> 16;

	ut64 addr = 0x20 - 4;
	ut32 word = 0;
	ut8 wordbuf[sizeof (word)];
	bool isBe = false;
	switch (mh->cputype) {
	case CPU_TYPE_POWERPC:
	case CPU_TYPE_POWERPC64:
		isBe = true;
		break;
	}
#define READWORD() \
		if (r_buf_read_at (buf, addr, (ut8*)wordbuf, 4) != 4) { \
			eprintf ("Invalid address in buffer."); \
			break; \
		} \
		addr += 4; \
		pvaddr += 4;\
		word = isBe? r_read_be32 (wordbuf): r_read_le32 (wordbuf);
	if (is64) {
		addr += 4;
		pvaddr += 4;
	}
	for (n = 0; n < mh->ncmds && addr < length; n++) {
		READWORD ();
		ut32 lcType = word;
		const char *pf_definition = cmd_to_pf_definition (lcType);
		if (pf_definition) {
			cb_printf ("pf.%s @ 0x%08"PFMT64x"\n", pf_definition, pvaddr - 4);
		}
		cb_printf ("0x%08"PFMT64x"  cmd %7d 0x%x %s\n",
			pvaddr - 4, n, lcType, cmd_to_string (lcType));
		READWORD ();
		if (addr > length) {
			break;
		}
		int lcSize = word;
		word &= 0xFFFFFF;
		cb_printf ("0x%08"PFMT64x"  cmdsize     %d\n", pvaddr - 4, word);
		if (lcSize < 1) {
			eprintf ("Invalid size for a load command\n");
			break;
		}
		switch (lcType) {
		case LC_BUILD_VERSION: {
			cb_printf ("0x%08"PFMT64x"  platform    %s\n",
				pvaddr, build_version_platform_to_string (r_buf_read_le32_at (buf, addr)));
			cb_printf ("0x%08"PFMT64x"  minos       %d.%d.%d\n",
				pvaddr + 4, r_buf_read_le16_at (buf, addr + 6), r_buf_read8_at (buf, addr + 5),
				r_buf_read8_at (buf, addr + 4));
			cb_printf ("0x%08"PFMT64x"  sdk         %d.%d.%d\n",
				pvaddr + 8, r_buf_read_le16_at (buf, addr + 10), r_buf_read8_at (buf, addr + 9),
				r_buf_read8_at (buf, addr + 8));
			ut32 ntools = r_buf_read_le32_at (buf, addr + 12);
			cb_printf ("0x%08"PFMT64x"  ntools      %d\n",
				pvaddr + 12, ntools);
			ut64 off = 16;
			while (off < (lcSize - 8) && ntools--) {
				cb_printf ("pf.mach0_build_version_tool @ 0x%08"PFMT64x"\n", pvaddr + off);
				cb_printf ("0x%08"PFMT64x"  tool        %s\n",
					pvaddr + off, build_version_tool_to_string (r_buf_read_le32_at (buf, addr + off)));
				off += 4;
				if (off >= (lcSize - 8)) {
					break;
				}
				cb_printf ("0x%08"PFMT64x"  version     %d.%d.%d\n",
					pvaddr + off, r_buf_read_le16_at (buf, addr + off + 2), r_buf_read8_at (buf, addr + off + 1),
					r_buf_read8_at (buf, addr + off));
				off += 4;
			}
			break;
		}
		case LC_MAIN:
			{
				ut8 data[64] = {0};
				r_buf_read_at (buf, addr, data, sizeof (data));
#if R_BIN_MACH064
				ut64 ep = r_read_ble64 (&data, false); //  bin->big_endian);
				cb_printf ("0x%08"PFMT64x"  entry0      0x%" PFMT64x "\n", pvaddr, ep);
				ut64 ss = r_read_ble64 (&data[8], false); //  bin->big_endian);
				cb_printf ("0x%08"PFMT64x"  stacksize   0x%" PFMT64x "\n", pvaddr +  8, ss);
#else
				ut32 ep = r_read_ble32 (&data, false); //  bin->big_endian);
				cb_printf ("0x%08"PFMT32x"  entry0      0x%" PFMT32x "\n", (ut32)pvaddr, ep);
				ut32 ss = r_read_ble32 (&data[4], false); //  bin->big_endian);
				cb_printf ("0x%08"PFMT32x"  stacksize   0x%" PFMT32x "\n", (ut32)pvaddr +  4, ss);
#endif
			}
			break;
		case LC_SYMTAB:
#if 0
			{
			char *id = r_buf_get_string (buf, addr + 20);
			cb_printf ("0x%08"PFMT64x"  id         0x%x\n", addr + 20, r_str_get (id));
			cb_printf ("0x%08"PFMT64x"  symooff    0x%x\n", addr + 20, r_str_get (id));
			cb_printf ("0x%08"PFMT64x"  nsyms      %d\n", addr + 20, r_str_get (id));
			cb_printf ("0x%08"PFMT64x"  stroff     0x%x\n", addr + 20, r_str_get (id));
			cb_printf ("0x%08"PFMT64x"  strsize    0x%x\n", addr + 20, r_str_get (id));
			free (id);
			}
#endif
			break;
		case LC_ID_DYLIB: { // install_name_tool
			ut32 str_off = r_buf_read_ble32_at (buf, addr, isBe);
			char *id = r_buf_get_string (buf, addr + str_off - 8);
			cb_printf ("0x%08"PFMT64x"  current     %d.%d.%d\n",
				pvaddr + 8, r_buf_read_le16_at (buf, addr + 10), r_buf_read8_at (buf, addr + 9),
				r_buf_read8_at (buf, addr + 8));
			cb_printf ("0x%08"PFMT64x"  compat      %d.%d.%d\n",
				pvaddr + 12, r_buf_read_le16_at (buf, addr + 14), r_buf_read8_at (buf, addr + 13),
				r_buf_read8_at (buf, addr + 12));
			cb_printf ("0x%08"PFMT64x"  id          %s\n",
				pvaddr + str_off - 8, r_str_get (id));
			if (id) {
				free (id);
			}
			break;
		}
		case LC_UUID:
			{
				ut8 i, uuid[16];
				r_buf_read_at (buf, addr, uuid, sizeof (uuid));
				cb_printf ("0x%08"PFMT64x"  uuid        ", pvaddr);
				for (i = 0; i < sizeof (uuid); i++) {
					cb_printf ("%02x", uuid[i]);
				}
				cb_printf ("\n");
			}
			break;
		case LC_SEGMENT:
		case LC_SEGMENT_64:
			{
				ut8 name[17] = {0};
				r_buf_read_at (buf, addr, name, sizeof (name) - 1);
				cb_printf ("0x%08"PFMT64x"  name        %s\n", pvaddr, name);
				ut32 nsects = r_buf_read_le32_at (buf, addr - 8 + (is64 ? 64 : 48));
				ut64 off = is64 ? 72 : 56;
				while (off < lcSize && nsects--) {
					if (is64) {
						cb_printf ("pf.mach0_section64 @ 0x%08"PFMT64x"\n", pvaddr - 8 + off);
						off += 80;
					} else {
						cb_printf ("pf.mach0_section @ 0x%08"PFMT64x"\n", pvaddr - 8 + off);
						off += 68;
					}
				}
			}
			break;
		case LC_LOAD_DYLIB:
		case LC_LOAD_WEAK_DYLIB: {
			ut32 str_off = r_buf_read_ble32_at (buf, addr, isBe);
			char *load_dylib = r_buf_get_string (buf, addr + str_off - 8);
			cb_printf ("0x%08"PFMT64x"  current     %d.%d.%d\n",
				pvaddr + 8, r_buf_read_le16_at (buf, addr + 10), r_buf_read8_at (buf, addr + 9),
				r_buf_read8_at (buf, addr + 8));
			cb_printf ("0x%08"PFMT64x"  compat      %d.%d.%d\n",
				pvaddr + 12, r_buf_read_le16_at (buf, addr + 14), r_buf_read8_at (buf, addr + 13),
				r_buf_read8_at (buf, addr + 12));
			cb_printf ("0x%08"PFMT64x"  load_dylib  %s\n",
				pvaddr + str_off - 8, r_str_get (load_dylib));
			if (load_dylib) {
				free (load_dylib);
			}
			break;
		}
		case LC_RPATH: {
			char *rpath = r_buf_get_string (buf, addr + 4);
			cb_printf ("0x%08" PFMT64x "  rpath       %s\n",
				pvaddr + 4, r_str_get (rpath));
			if (rpath) {
				free (rpath);
			}
			break;
		}
		case LC_ENCRYPTION_INFO:
		case LC_ENCRYPTION_INFO_64: {
			ut32 word = r_buf_read_le32_at (buf, addr);
			cb_printf ("0x%08"PFMT64x"  cryptoff   0x%08x\n", pvaddr, word);
			word = r_buf_read_le32_at (buf, addr + 4);
			cb_printf ("0x%08"PFMT64x"  cryptsize  %d\n", pvaddr + 4, word);
			word = r_buf_read_le32_at (buf, addr + 8);
			cb_printf ("0x%08"PFMT64x"  cryptid    %d\n", pvaddr + 8, word);
			break;
		}
		case LC_CODE_SIGNATURE: {
			ut32 words[2];
			r_buf_read_at (buf, addr, (ut8 *)words, sizeof (words));
			cb_printf ("0x%08"PFMT64x"  dataoff     0x%08x\n", pvaddr, words[0]);
			cb_printf ("0x%08"PFMT64x"  datasize    %d\n", pvaddr + 4, words[1]);
			cb_printf ("# wtf mach0.sign %d @ 0x%x\n", words[1], words[0]);
			break;
		}
		}
		addr += word - 8;
		pvaddr += word - 8;
	}
	free (mh);
}