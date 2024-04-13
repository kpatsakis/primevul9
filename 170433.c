static bool parse_chained_fixups(struct MACH0_(obj_t) * bin, ut32 offset, ut32 size) {
	struct dyld_chained_fixups_header header;
	if (size < sizeof(header)) {
		return false;
	}
	if (rz_buf_fread_at(bin->b, offset, (ut8 *)&header, "7i", 1) != sizeof(header)) {
		return false;
	}
	if (header.fixups_version > 0) {
		eprintf("Unsupported fixups version: %u\n", header.fixups_version);
		return false;
	}
	ut64 starts_at = offset + header.starts_offset;
	if (header.starts_offset > size) {
		return false;
	}
	if (!rz_buf_read_le32_at(bin->b, starts_at, &bin->nchained_starts)) {
		return false;
	}
	bin->chained_starts = RZ_NEWS0(struct rz_dyld_chained_starts_in_segment *, bin->nchained_starts);
	if (!bin->chained_starts) {
		return false;
	}
	size_t i;
	ut64 cursor = starts_at + sizeof(ut32);
	for (i = 0; i < bin->nchained_starts; i++) {
		ut32 seg_off;
		if (!rz_buf_read_le32_at(bin->b, cursor, &seg_off) || !seg_off) {
			cursor += sizeof(ut32);
			continue;
		}
		if (i >= bin->nsegs) {
			break;
		}
		struct rz_dyld_chained_starts_in_segment *cur_seg = RZ_NEW0(struct rz_dyld_chained_starts_in_segment);
		if (!cur_seg) {
			return false;
		}
		bin->chained_starts[i] = cur_seg;
		if (rz_buf_fread_at(bin->b, starts_at + seg_off, (ut8 *)cur_seg, "isslis", 1) != 22) {
			return false;
		}
		if (cur_seg->page_count > 0) {
			ut16 *page_start = malloc(sizeof(ut16) * cur_seg->page_count);
			if (!page_start) {
				return false;
			}
			if (rz_buf_fread_at(bin->b, starts_at + seg_off + 22, (ut8 *)page_start, "s", cur_seg->page_count) != cur_seg->page_count * 2) {
				return false;
			}
			cur_seg->page_start = page_start;
		}
		cursor += sizeof(ut32);
	}
	/* TODO: handle also imports, symbols and multiple starts (32-bit only) */
	return true;
}