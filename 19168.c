R_API int r_core_search_value_in_range(RCore *core, RInterval search_itv, ut64 vmin,
					 ut64 vmax, int vsize, inRangeCb cb, void *cb_user) {
	int i, align = core->search->align, hitctr = 0;
	bool vinfun = r_config_get_i (core->config, "anal.vinfun");
	bool vinfunr = r_config_get_i (core->config, "anal.vinfunrange");
	bool analStrings = r_config_get_i (core->config, "anal.strings");
	mycore = core;
	ut8 buf[4096];
	ut64 v64, value = 0, size;
	ut64 from = search_itv.addr, to = r_itv_end (search_itv);
	ut32 v32;
	ut16 v16;
	if (from >= to) {
		eprintf ("Error: from must be lower than to\n");
		return -1;
	}
	bool maybeThumb = false;
	if (align && core->anal->cur && core->anal->cur->arch) {
		if (!strcmp (core->anal->cur->arch, "arm") && core->anal->bits != 64) {
			maybeThumb = true;
		}
	}

	if (vmin >= vmax) {
		eprintf ("Error: vmin must be lower than vmax\n");
		return -1;
	}
	if (to == UT64_MAX) {
		eprintf ("Error: Invalid destination boundary\n");
		return -1;
	}
	r_cons_break_push (NULL, NULL);

	if (!r_io_is_valid_offset (core->io, from, 0)) {
		return -1;
	}
	while (from < to) {
		size = R_MIN (to - from, sizeof (buf));
		memset (buf, 0xff, sizeof (buf)); // probably unnecessary
		if (r_cons_is_breaked ()) {
			goto beach;
		}
		bool res = r_io_read_at_mapped (core->io, from, buf, size);
		if (!res || !memcmp (buf, "\xff\xff\xff\xff", 4) || !memcmp (buf, "\x00\x00\x00\x00", 4)) {
			if (!isValidAddress (core, from)) {
				ut64 next = from;
				if (!r_io_map_locate (core->io, &next, 1, 0)) {
					from += sizeof (buf);
				} else {
					from += (next - from);
				}
				continue;
			}
		}
		for (i = 0; i <= (size - vsize); i++) {
			void *v = (buf + i);
			ut64 addr = from + i;
			if (r_cons_is_breaked ()) {
				goto beach;
			}
			if (align && (addr) % align) {
				continue;
			}
			int match = false;
			int left = size - i;
			if (vsize > left) {
				break;
			}
			switch (vsize) {
			case 1: value = *(ut8 *)v; match = (buf[i] >= vmin && buf[i] <= vmax); break;
			case 2: v16 = *(uut16 *)v; match = (v16 >= vmin && v16 <= vmax); value = v16; break;
			case 4: v32 = *(uut32 *)v; match = (v32 >= vmin && v32 <= vmax); value = v32; break;
			case 8: v64 = *(uut64 *)v; match = (v64 >= vmin && v64 <= vmax); value = v64; break;
			default: eprintf ("Unknown vsize %d\n", vsize); return -1;
			}
			if (match && !vinfun) {
				if (vinfunr) {
					if (r_anal_get_fcn_in_bounds (core->anal, addr, R_ANAL_FCN_TYPE_NULL)) {
						match = false;
					}
				} else {
					if (r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL)) {
						match = false;
					}
				}
			}
			if (match && value) {
				bool isValidMatch = true;
				if (align && (value % align)) {
					// ignored .. unless we are analyzing arm/thumb and lower bit is 1
					isValidMatch = false;
					if (maybeThumb && (value & 1)) {
						isValidMatch = true;
					}
				}
				if (isValidMatch) {
					cb (core, addr, value, vsize, cb_user);
					if (analStrings && stringAt (core, addr)) {
						add_string_ref (mycore, addr, value);
					}
					hitctr++;
				}
			}
		}
		if (size == to-from) {
			break;
		}
		from += size-vsize+1;
	}
beach:
	r_cons_break_pop ();
	return hitctr;
}