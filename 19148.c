static bool r_anal_try_get_fcn(RCore *core, RAnalRef *ref, int fcndepth, int refdepth) {
	if (!refdepth) {
		return false;
	}
	RIOMap *map = r_io_map_get_at (core->io, ref->addr);
	if (!map) {
		return false;
	}

	if (map->perm & R_PERM_X) {
		ut8 buf[64];
		r_io_read_at (core->io, ref->addr, buf, sizeof (buf));
		bool looksLikeAFunction = r_anal_check_fcn (core->anal, buf, sizeof (buf), ref->addr, r_io_map_begin (map),
				r_io_map_end (map));
		if (looksLikeAFunction) {
			if (core->anal->limit) {
				if (ref->addr < core->anal->limit->from ||
						ref->addr > core->anal->limit->to) {
					return 1;
				}
			}
			r_core_anal_fcn (core, ref->addr, ref->at, ref->type, fcndepth - 1);
		}
	} else {
		ut64 offs = 0;
		ut64 sz = core->anal->bits >> 3;
		RAnalRef ref1;
		ref1.type = R_ANAL_REF_TYPE_DATA;
		ref1.at = ref->addr;
		ref1.addr = 0;
		ut32 i32;
		ut16 i16;
		ut8 i8;
		ut64 offe = offs + 1024;
		for (offs = 0; offs < offe; offs += sz, ref1.at += sz) {
			ut8 bo[8];
			r_io_read_at (core->io, ref->addr + offs, bo, R_MIN (sizeof (bo), sz));
			bool be = core->anal->big_endian;
			switch (sz) {
			case 1:
				i8 = r_read_ble8 (bo);
				ref1.addr = (ut64)i8;
				break;
			case 2:
				i16 = r_read_ble16 (bo, be);
				ref1.addr = (ut64)i16;
				break;
			case 4:
				i32 = r_read_ble32 (bo, be);
				ref1.addr = (ut64)i32;
				break;
			case 8:
				ref1.addr = r_read_ble64 (bo, be);
				break;
			}
			r_anal_try_get_fcn (core, &ref1, fcndepth, refdepth - 1);
		}
	}
	return 1;
}