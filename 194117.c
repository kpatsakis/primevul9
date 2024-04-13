
static void cli_parseres_special(uint32_t base, uint32_t rva, fmap_t *map, struct cli_exe_section *exe_sections, uint16_t nsections, size_t fsize, uint32_t hdr_size, unsigned int level, uint32_t type, unsigned int *maxres, struct swizz_stats *stats) {
    unsigned int err = 0, i;
    uint8_t *resdir;
    uint8_t *entry, *oentry;
    uint16_t named, unnamed;
    uint32_t rawaddr = cli_rawaddr(rva, exe_sections, nsections, &err, fsize, hdr_size);
    uint32_t entries;

    if(level>2 || !*maxres) return;
    *maxres-=1;
    if(err || !(resdir = fmap_need_off_once(map, rawaddr, 16)))
	    return;
    named = (uint16_t)cli_readint16(resdir+12);
    unnamed = (uint16_t)cli_readint16(resdir+14);

    entries = /*named+*/unnamed;
    if (!entries)
	    return;
    rawaddr += named*8; /* skip named */
    /* this is just used in a heuristic detection, so don't give error on failure */
    if(!(entry = fmap_need_off(map, rawaddr+16, entries*8))) {
	    cli_dbgmsg("cli_parseres_special: failed to read resource directory at:%lu\n", (unsigned long)rawaddr+16);
	    return;
    }
    oentry = entry;
    /*for (i=0; i<named; i++) {
	uint32_t id, offs;
	id = cli_readint32(entry);
	offs = cli_readint32(entry+4);
	if(offs>>31)
	    cli_parseres( base, base + (offs&0x7fffffff), srcfd, exe_sections, nsections, fsize, hdr_size, level+1, type, maxres, stats);
	entry+=8;
    }*/
    for (i=0; i<unnamed; i++, entry += 8) {
	uint32_t id, offs;
	if (stats->errors >= SWIZZ_MAXERRORS) {
	    cli_dbgmsg("cli_parseres_special: resources broken, ignoring\n");
	    return;
	}
	id = cli_readint32(entry)&0x7fffffff;
	if(level==0) {
		type = 0;
		switch(id) {
			case 4: /* menu */
			case 5: /* dialog */
			case 6: /* string */
			case 11:/* msgtable */
				type = id;
				break;
			case 16:
				type = id;
				/* 14: version */
				stats->has_version = 1;
				break;
			case 24: /* manifest */
				stats->has_manifest = 1;
				break;
			/* otherwise keep it 0, we don't want it */
		}
	}
	if (!type) {
		/* if we are not interested in this type, skip */
		continue;
	}
	offs = cli_readint32(entry+4);
	if(offs>>31)
		cli_parseres_special(base, base + (offs&0x7fffffff), map, exe_sections, nsections, fsize, hdr_size, level+1, type, maxres, stats);
	else {
			offs = cli_readint32(entry+4);
			rawaddr = cli_rawaddr(base + offs, exe_sections, nsections, &err, fsize, hdr_size);
			if (!err && (resdir = fmap_need_off_once(map, rawaddr, 16))) {
				uint32_t isz = cli_readint32(resdir+4);
				uint8_t *str;
				rawaddr = cli_rawaddr(cli_readint32(resdir), exe_sections, nsections, &err, fsize, hdr_size);
				if (err || !isz || isz >= fsize || rawaddr+isz >= fsize) {
					cli_dbgmsg("cli_parseres_special: invalid resource table entry: %lu + %lu\n", 
							(unsigned long)rawaddr, 
							(unsigned long)isz);
					stats->errors++;
					continue;
				}
				if ((id&0xff) != 0x09) /* english res only */
				    continue;
				if((str = fmap_need_off_once(map, rawaddr, isz)))
					cli_detect_swizz_str(str, isz, stats, type);
			}
	}
    }
    fmap_unneed_ptr(map, oentry, entries*8);