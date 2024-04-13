RzList *MACH0_(get_segments)(RzBinFile *bf) {
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	if (bin->sections_cache) {
		return bin->sections_cache;
	}
	RzList *list = rz_list_newf((RzListFree)rz_bin_section_free);
	size_t i, j;

	if (bin->nsegs > 0) {
		struct MACH0_(segment_command) * seg;
		for (i = 0; i < bin->nsegs; i++) {
			seg = &bin->segs[i];
			if (!seg->initprot) {
				continue;
			}
			RzBinSection *s = rz_bin_section_new(NULL);
			if (!s) {
				break;
			}
			s->vaddr = seg->vmaddr;
			s->vsize = seg->vmsize;
			s->size = seg->vmsize;
			s->paddr = seg->fileoff;
			s->paddr += bf->o->boffset;
			// TODO s->flags = seg->flags;
			s->name = rz_str_ndup(seg->segname, 16);
			s->is_segment = true;
			rz_str_filter(s->name);
			s->perm = prot2perm(seg->initprot);
			rz_list_append(list, s);
		}
	}
	if (bin->nsects > 0) {
		int last_section = RZ_MIN(bin->nsects, 128); // maybe drop this limit?
		for (i = 0; i < last_section; i++) {
			RzBinSection *s = RZ_NEW0(RzBinSection);
			if (!s) {
				break;
			}
			s->vaddr = (ut64)bin->sects[i].addr;
			s->vsize = (ut64)bin->sects[i].size;
			s->align = (ut64)(1ULL << (bin->sects[i].align & 63));
			s->is_segment = false;
			s->size = (bin->sects[i].flags == S_ZEROFILL) ? 0 : (ut64)bin->sects[i].size;
			// The bottom byte of flags is the section type
			s->type = bin->sects[i].flags & 0xFF;
			s->flags = bin->sects[i].flags & 0xFFFFFF00;
			// XXX flags
			s->paddr = (ut64)bin->sects[i].offset;
			int segment_index = 0;
			// s->perm =prot2perm (bin->segs[j].initprot);
			for (j = 0; j < bin->nsegs; j++) {
				if (s->vaddr >= bin->segs[j].vmaddr &&
					s->vaddr < (bin->segs[j].vmaddr + bin->segs[j].vmsize)) {
					s->perm = prot2perm(bin->segs[j].initprot);
					segment_index = j;
					break;
				}
			}
			char *section_name = rz_str_ndup(bin->sects[i].sectname, 16);
			char *segment_name = rz_str_newf("%zu.%s", i, bin->segs[segment_index].segname);
			s->name = rz_str_newf("%s.%s", segment_name, section_name);
			s->is_data = __isDataSection(s);
			if (strstr(section_name, "interpos") || strstr(section_name, "__mod_")) {
#if RZ_BIN_MACH064
				const int ws = 8;
#else
				const int ws = 4;
#endif
				s->format = rz_str_newf("Cd %d[%" PFMT64d "]", ws, s->vsize / ws);
			}
			rz_list_append(list, s);
			free(segment_name);
			free(section_name);
		}
	}
	bin->sections_cache = list;
	return list;
}