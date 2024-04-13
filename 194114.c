*/
void findres(uint32_t by_type, uint32_t by_name, uint32_t res_rva, fmap_t *map, struct cli_exe_section *exe_sections, uint16_t nsections, uint32_t hdr_size, int (*cb)(void *, uint32_t, uint32_t, uint32_t, uint32_t), void *opaque) {
    unsigned int err = 0;
    uint32_t type, type_offs, name, name_offs, lang, lang_offs;
    uint8_t *resdir, *type_entry, *name_entry, *lang_entry ;
    uint16_t type_cnt, name_cnt, lang_cnt;

    if (!(resdir = fmap_need_off_once(map, cli_rawaddr(res_rva, exe_sections, nsections, &err, map->len, hdr_size), 16)) || err)
	return;

    type_cnt = (uint16_t)cli_readint16(resdir+12);
    type_entry = resdir+16;
    if(!(by_type>>31)) {
	type_entry += type_cnt * 8;
	type_cnt = (uint16_t)cli_readint16(resdir+14);
    }

    while(type_cnt--) {
	if(!fmap_need_ptr_once(map, type_entry, 8))
	    return;
	type = cli_readint32(type_entry);
	type_offs = cli_readint32(type_entry+4);
	if(type == by_type && (type_offs>>31)) {
	    type_offs &= 0x7fffffff;
	    if (!(resdir = fmap_need_off_once(map, cli_rawaddr(res_rva + type_offs, exe_sections, nsections, &err, map->len, hdr_size), 16)) || err)
		return;

	    name_cnt = (uint16_t)cli_readint16(resdir+12);
	    name_entry = resdir+16;
	    if(by_name == 0xffffffff)
		name_cnt += (uint16_t)cli_readint16(resdir+14);
	    else if(!(by_name>>31)) {
		name_entry += name_cnt * 8;
		name_cnt = (uint16_t)cli_readint16(resdir+14);
	    }
	    while(name_cnt--) {
		if(!fmap_need_ptr_once(map, name_entry, 8))
		    return;
		name = cli_readint32(name_entry);
		name_offs = cli_readint32(name_entry+4);
		if((by_name == 0xffffffff || name == by_name) && (name_offs>>31)) {
		    name_offs &= 0x7fffffff;
		    if (!(resdir = fmap_need_off_once(map, cli_rawaddr(res_rva + name_offs, exe_sections, nsections, &err, map->len, hdr_size), 16)) || err)
			return;
		    
		    lang_cnt = (uint16_t)cli_readint16(resdir+12) + (uint16_t)cli_readint16(resdir+14);
		    lang_entry = resdir+16;
		    while(lang_cnt--) {
			if(!fmap_need_ptr_once(map, lang_entry, 8))
			    return;
			lang = cli_readint32(lang_entry);
			lang_offs = cli_readint32(lang_entry+4);
			if(!(lang_offs >>31)) {
			    if(cb(opaque, type, name, lang, res_rva + lang_offs))
				return;
			}
			lang_entry += 8;
		    }
		}
		name_entry += 8;
	    }
	    return; /* FIXME: unless we want to find ALL types */
	}
	type_entry += 8;
    }