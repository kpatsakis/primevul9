
int cli_peheader(fmap_t *map, struct cli_exe_info *peinfo)
{
	uint16_t e_magic; /* DOS signature ("MZ") */
	uint32_t e_lfanew; /* address of new exe header */
	/* Obsolete - see below
	  uint32_t min = 0, max = 0;
	*/
	struct pe_image_file_hdr file_hdr;
	union {
	    struct pe_image_optional_hdr64 opt64;
	    struct pe_image_optional_hdr32 opt32;
	} pe_opt;
	struct pe_image_section_hdr *section_hdr;
	int i;
	unsigned int err, pe_plus = 0;
	uint32_t valign, falign, hdr_size;
	size_t fsize;
	ssize_t at;
	struct pe_image_data_dir *dirs;

    cli_dbgmsg("in cli_peheader\n");

    fsize = map->len - peinfo->offset;
    if(fmap_readn(map, &e_magic, peinfo->offset, sizeof(e_magic)) != sizeof(e_magic)) {
	cli_dbgmsg("Can't read DOS signature\n");
	return -1;
    }

    if(EC16(e_magic) != PE_IMAGE_DOS_SIGNATURE && EC16(e_magic) != PE_IMAGE_DOS_SIGNATURE_OLD) {
	cli_dbgmsg("Invalid DOS signature\n");
	return -1;
    }

    if(fmap_readn(map, &e_lfanew, peinfo->offset + 58 + sizeof(e_magic), sizeof(e_lfanew)) != sizeof(e_lfanew)) {
	/* truncated header? */
	return -1;
    }

    e_lfanew = EC32(e_lfanew);
    if(!e_lfanew) {
	cli_dbgmsg("Not a PE file\n");
	return -1;
    }

    if(fmap_readn(map, &file_hdr, peinfo->offset + e_lfanew, sizeof(struct pe_image_file_hdr)) != sizeof(struct pe_image_file_hdr)) {
	/* bad information in e_lfanew - probably not a PE file */
	cli_dbgmsg("Can't read file header\n");
	return -1;
    }

    if(EC32(file_hdr.Magic) != PE_IMAGE_NT_SIGNATURE) {
	cli_dbgmsg("Invalid PE signature (probably NE file)\n");
	return -1;
    }

    if ( (peinfo->nsections = EC16(file_hdr.NumberOfSections)) < 1 || peinfo->nsections > 96 ) return -1;

    if (EC16(file_hdr.SizeOfOptionalHeader) < sizeof(struct pe_image_optional_hdr32)) {
        cli_dbgmsg("SizeOfOptionalHeader too small\n");
	return -1;
    }

    at = peinfo->offset + e_lfanew + sizeof(struct pe_image_file_hdr);
    if(fmap_readn(map, &optional_hdr32, at, sizeof(struct pe_image_optional_hdr32)) != sizeof(struct pe_image_optional_hdr32)) {
        cli_dbgmsg("Can't read optional file header\n");
	return -1;
    }
    at += sizeof(struct pe_image_optional_hdr32);

    if(EC16(optional_hdr64.Magic)==PE32P_SIGNATURE) { /* PE+ */
        if(EC16(file_hdr.SizeOfOptionalHeader)!=sizeof(struct pe_image_optional_hdr64)) {
	    cli_dbgmsg("Incorrect SizeOfOptionalHeader for PE32+\n");
	    return -1;
	}
	if(fmap_readn(map, &optional_hdr32 + 1, at, sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32)) != sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32)) {
	    cli_dbgmsg("Can't read optional file header\n");
	    return -1;
	}
	at += sizeof(struct pe_image_optional_hdr64) - sizeof(struct pe_image_optional_hdr32);
	hdr_size = EC32(optional_hdr64.SizeOfHeaders);
	pe_plus=1;
    } else { /* PE */
	if (EC16(file_hdr.SizeOfOptionalHeader)!=sizeof(struct pe_image_optional_hdr32)) {
	    /* Seek to the end of the long header */
	    at += EC16(file_hdr.SizeOfOptionalHeader)-sizeof(struct pe_image_optional_hdr32);
	}
	hdr_size = EC32(optional_hdr32.SizeOfHeaders);
    }

    valign = (pe_plus)?EC32(optional_hdr64.SectionAlignment):EC32(optional_hdr32.SectionAlignment);
    falign = (pe_plus)?EC32(optional_hdr64.FileAlignment):EC32(optional_hdr32.FileAlignment);

    peinfo->hdr_size = hdr_size = PESALIGN(hdr_size, valign);

    peinfo->section = (struct cli_exe_section *) cli_calloc(peinfo->nsections, sizeof(struct cli_exe_section));

    if(!peinfo->section) {
	cli_dbgmsg("Can't allocate memory for section headers\n");
	return -1;
    }

    section_hdr = (struct pe_image_section_hdr *) cli_calloc(peinfo->nsections, sizeof(struct pe_image_section_hdr));

    if(!section_hdr) {
	cli_dbgmsg("Can't allocate memory for section headers\n");
	free(peinfo->section);
	peinfo->section = NULL;
	return -1;
    }

    if(fmap_readn(map, section_hdr, at, peinfo->nsections * sizeof(struct pe_image_section_hdr)) != peinfo->nsections * sizeof(struct pe_image_section_hdr)) {
        cli_dbgmsg("Can't read section header\n");
	cli_dbgmsg("Possibly broken PE file\n");
	free(section_hdr);
	free(peinfo->section);
	peinfo->section = NULL;
	return -1;
    }
    at += sizeof(struct pe_image_section_hdr)*peinfo->nsections;

    for(i = 0; falign!=0x200 && i<peinfo->nsections; i++) {
	/* file alignment fallback mode - blah */
	if (falign && section_hdr[i].SizeOfRawData && EC32(section_hdr[i].PointerToRawData)%falign && !(EC32(section_hdr[i].PointerToRawData)%0x200)) {
	    falign = 0x200;
	}
    }

    for(i = 0; i < peinfo->nsections; i++) {
        peinfo->section[i].rva = PEALIGN(EC32(section_hdr[i].VirtualAddress), valign);
	peinfo->section[i].vsz = PESALIGN(EC32(section_hdr[i].VirtualSize), valign);
	peinfo->section[i].raw = PEALIGN(EC32(section_hdr[i].PointerToRawData), falign);
	peinfo->section[i].rsz = PESALIGN(EC32(section_hdr[i].SizeOfRawData), falign);

	if (!peinfo->section[i].vsz && peinfo->section[i].rsz)
	    peinfo->section[i].vsz=PESALIGN(EC32(section_hdr[i].SizeOfRawData), valign);

	if (peinfo->section[i].rsz && !CLI_ISCONTAINED(0, (uint32_t) fsize, peinfo->section[i].raw, peinfo->section[i].rsz))
	    peinfo->section[i].rsz = (fsize - peinfo->section[i].raw)*(fsize>peinfo->section[i].raw);
    }

    if(pe_plus) {
	peinfo->ep = EC32(optional_hdr64.AddressOfEntryPoint);
	dirs = optional_hdr64.DataDirectory;
    } else {
	peinfo->ep = EC32(optional_hdr32.AddressOfEntryPoint);
	dirs = optional_hdr32.DataDirectory;
    }

    if(!(peinfo->ep = cli_rawaddr(peinfo->ep, peinfo->section, peinfo->nsections, &err, fsize, hdr_size)) && err) {
	cli_dbgmsg("Broken PE file\n");
	free(section_hdr);
	free(peinfo->section);
	peinfo->section = NULL;
	return -1;
    }

    if(EC16(file_hdr.Characteristics) & 0x2000 || !dirs[2].Size)
	peinfo->res_addr = 0;
    else
	peinfo->res_addr = EC32(dirs[2].VirtualAddress);

    while(dirs[2].Size) {
	struct vinfo_list vlist;
	uint8_t *vptr, *baseptr;
    	uint32_t rva, res_sz;

	memset(&vlist, 0, sizeof(vlist));
    	findres(0x10, 0xffffffff, EC32(dirs[2].VirtualAddress), map, peinfo->section, peinfo->nsections, hdr_size, versioninfo_cb, &vlist);
	if(!vlist.count) break; /* No version_information */
	if(cli_hashset_init(&peinfo->vinfo, 32, 80)) {
	    cli_errmsg("cli_peheader: Unable to init vinfo hashset\n");
	    free(section_hdr);
	    free(peinfo->section);
	    peinfo->section = NULL;
	    return -1;
	}

	err = 0;
	for(i=0; i<vlist.count; i++) { /* enum all version_information res - RESUMABLE */
	    cli_dbgmsg("cli_peheader: parsing version info @ rva %x (%u/%u)\n", vlist.rvas[i], i+1, vlist.count);
	    rva = cli_rawaddr(vlist.rvas[i], peinfo->section, peinfo->nsections, &err, fsize, hdr_size);
	    if(err)
		continue;

	    if(!(vptr = fmap_need_off_once(map, rva, 16)))
		continue;

	    baseptr = vptr - rva;
	    /* parse resource */
	    rva = cli_readint32(vptr); /* ptr to version_info */
	    res_sz = cli_readint32(vptr+4); /* sizeof(resource) */
	    rva = cli_rawaddr(rva, peinfo->section, peinfo->nsections, &err, fsize, hdr_size);
	    if(err)
		continue;
	    if(!(vptr = fmap_need_off_once(map, rva, res_sz)))
		continue;
	    
	    while(res_sz>4) { /* look for version_info - NOT RESUMABLE (expecting exactly one versioninfo) */
		uint32_t vinfo_sz, vinfo_val_sz, got_varfileinfo = 0;

		vinfo_sz = vinfo_val_sz = cli_readint32(vptr);
		vinfo_sz &= 0xffff;
		if(vinfo_sz > res_sz)
		    break; /* the content is larger than the container */

		vinfo_val_sz >>= 16;
		if(vinfo_sz <= 6 + 0x20 + 2 + 0x34 ||
		   vinfo_val_sz != 0x34 || 
		   memcmp(vptr+6, "V\0S\0_\0V\0E\0R\0S\0I\0O\0N\0_\0I\0N\0F\0O\0\0\0", 0x20) ||
		   cli_readint32(vptr + 0x28) != 0xfeef04bd) {
		    /* - there should be enough room for the header(6), the key "VS_VERSION_INFO"(20), the padding(2) and the value(34)
		     * - the value should be sizeof(fixedfileinfo)
		     * - the key should match
		     * - there should be some proper magic for fixedfileinfo */
		    break; /* there's no point in looking further */
		}

		/* move to the end of fixedfileinfo where the child elements are located */
		vptr += 6 + 0x20 + 2 + 0x34;
		vinfo_sz -= 6 + 0x20 + 2 + 0x34;

		while(vinfo_sz > 6) { /* look for stringfileinfo - NOT RESUMABLE (expecting at most one stringfileinfo) */
		    uint32_t sfi_sz = cli_readint32(vptr) & 0xffff;

		    if(sfi_sz > vinfo_sz)
			break; /* the content is larger than the container */

		    if(!got_varfileinfo && sfi_sz > 6 + 0x18 && !memcmp(vptr+6, "V\0a\0r\0F\0i\0l\0e\0I\0n\0f\0o\0\0\0", 0x18)) {
			/* skip varfileinfo as it sometimes appear before stringtableinfo */
			vptr += sfi_sz;
			vinfo_sz -= sfi_sz;
			got_varfileinfo = 1;
			continue;
		    }

		    if(sfi_sz <= 6 + 0x1e || memcmp(vptr+6, "S\0t\0r\0i\0n\0g\0F\0i\0l\0e\0I\0n\0f\0o\0\0\0", 0x1e)) {
			/* - there should be enough room for the header(6) and the key "StringFileInfo"(1e)
			 * - the key should match */
			break; /* this is an implicit hard fail: parent is not resumable */
		    }

		    /* move to the end of stringfileinfo where the child elements are located */
		    vptr += 6 + 0x1e;
		    sfi_sz -= 6 + 0x1e;

		    while(sfi_sz > 6) { /* enum all stringtables - RESUMABLE */
			uint32_t st_sz = cli_readint32(vptr) & 0xffff;
			uint8_t *next_vptr = vptr + st_sz;
			uint32_t next_sfi_sz = sfi_sz - st_sz;

			if(st_sz > sfi_sz || st_sz <= 24) {
			    /* - the content is larger than the container
			       - there's no room for a stringtables (headers(6) + key(16) + padding(2)) */
			    break; /* this is an implicit hard fail: parent is not resumable */
			}

			/* move to the end of stringtable where the child elements are located */
			vptr += 24;
			st_sz -= 24;

			while(st_sz > 6) {  /* enum all strings - RESUMABLE */
			    uint32_t s_sz, s_key_sz, s_val_sz;

			    s_sz = (cli_readint32(vptr) & 0xffff) + 3;
			    s_sz &= ~3;
			    if(s_sz > st_sz || s_sz <= 6 + 2 + 8) {
				/* - the content is larger than the container
				 * - there's no room for a minimal string
				 * - there's no room for the value */
				st_sz = 0;
				sfi_sz = 0;
				break; /* force a hard fail */
			    }

			    /* ~wcstrlen(key) */
			    for(s_key_sz = 6; s_key_sz+1 < s_sz; s_key_sz += 2) {
				if(vptr[s_key_sz] || vptr[s_key_sz+1]) continue;
				s_key_sz += 2;
				break;
			    }

			    s_key_sz += 3;
			    s_key_sz &= ~3;

			    if(s_key_sz >= s_sz) {
				/* key overflow */
				vptr += s_sz;
				st_sz -= s_sz;
				continue;
			    }

			    s_val_sz = s_sz - s_key_sz;
			    s_key_sz -= 6;

			    if(s_val_sz <= 2) {
				/* skip unset value */
				vptr += s_sz;
				st_sz -= s_sz;
				continue;
			    }

			    if(cli_hashset_addkey(&peinfo->vinfo, (uint32_t)(vptr - baseptr + 6))) {
				cli_errmsg("cli_peheader: Unable to add rva to vinfo hashset\n");
				cli_hashset_destroy(&peinfo->vinfo);
				free(section_hdr);
				free(peinfo->section);
				peinfo->section = NULL;
				return -1;
			    }

			    if(cli_debug_flag) {
				char *k, *v, *s;

				/* FIXME: skip too long strings */
				k = cli_utf16toascii((const char*)vptr + 6, s_key_sz);
				if(k) {
				    v = cli_utf16toascii((const char*)vptr + s_key_sz + 6, s_val_sz);
				    if(v) {
					s = cli_str2hex((const char*)vptr + 6, s_key_sz + s_val_sz - 6);
					if(s) {
					    cli_dbgmsg("VersionInfo (%x): '%s'='%s' - VI:%s\n", (uint32_t)(vptr - baseptr + 6), k, v, s);
					    free(s);
					}
					free(v);
				    }
				    free(k);
				}
			    }
			    vptr += s_sz;
			    st_sz -= s_sz;
			} /* enum all strings - RESUMABLE */
			vptr = next_vptr;
			sfi_sz = next_sfi_sz * (sfi_sz != 0);
		    } /* enum all stringtables - RESUMABLE */
		    break;
		} /* look for stringfileinfo - NOT RESUMABLE */
		break;
	    } /* look for version_info - NOT RESUMABLE */
	} /* enum all version_information res - RESUMABLE */
	break;
    } /* while(dirs[2].Size) */

    free(section_hdr);
    return 0;