struct section_t *MACH0_(get_sections)(struct MACH0_(obj_t) * bin) {
	rz_return_val_if_fail(bin, NULL);
	struct section_t *sections;
	char sectname[64], raw_segname[17];
	size_t i, j, to;

	/* for core files */
	if (bin->nsects < 1 && bin->nsegs > 0) {
		struct MACH0_(segment_command) * seg;
		if (!(sections = calloc((bin->nsegs + 1), sizeof(struct section_t)))) {
			return NULL;
		}
		for (i = 0; i < bin->nsegs; i++) {
			seg = &bin->segs[i];
			sections[i].addr = seg->vmaddr;
			sections[i].offset = seg->fileoff;
			sections[i].size = seg->vmsize;
			sections[i].vsize = seg->vmsize;
			sections[i].align = 4096;
			sections[i].flags = seg->flags;
			rz_strf(sectname, "%.16s", seg->segname);
			sectname[16] = 0;
			rz_str_filter(sectname);
			// hack to support multiple sections with same name
			sections[i].perm = prot2perm(seg->initprot);
			sections[i].last = 0;
		}
		sections[i].last = 1;
		return sections;
	}

	if (!bin->sects) {
		return NULL;
	}
	to = RZ_MIN(bin->nsects, 128); // limit number of sections here to avoid fuzzed bins
	if (to < 1) {
		return NULL;
	}
	if (!(sections = calloc(bin->nsects + 1, sizeof(struct section_t)))) {
		return NULL;
	}
	for (i = 0; i < to; i++) {
		sections[i].offset = (ut64)bin->sects[i].offset;
		sections[i].addr = (ut64)bin->sects[i].addr;
		sections[i].size = (bin->sects[i].flags == S_ZEROFILL) ? 0 : (ut64)bin->sects[i].size;
		sections[i].vsize = (ut64)bin->sects[i].size;
		sections[i].align = bin->sects[i].align;
		sections[i].flags = bin->sects[i].flags;
		rz_strf(sectname, "%.16s", bin->sects[i].sectname);
		rz_str_filter(sectname);
		rz_strf(raw_segname, "%.16s", bin->sects[i].segname);
		for (j = 0; j < bin->nsegs; j++) {
			if (sections[i].addr >= bin->segs[j].vmaddr &&
				sections[i].addr < (bin->segs[j].vmaddr + bin->segs[j].vmsize)) {
				sections[i].perm = prot2perm(bin->segs[j].initprot);
				break;
			}
		}
		snprintf(sections[i].name, sizeof(sections[i].name),
			"%d.%s.%s", (int)i, raw_segname, sectname);
		sections[i].last = 0;
	}
	sections[i].last = 1;
	return sections;
}