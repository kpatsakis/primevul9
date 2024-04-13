static bool anal_is_bad_call(RCore *core, ut64 from, ut64 to, ut64 addr, ut8 *buf, int bufi) {
	ut64 align = R_ABS (addr % PE_ALIGN);
	ut32 call_bytes;

	// XXX this is x86 specific
	if (align == 0) {
		call_bytes = (ut32)((ut8*)buf)[bufi + 3] << 24;
		call_bytes |= (ut32)((ut8*)buf)[bufi + 2] << 16;
		call_bytes |= (ut32)((ut8*)buf)[bufi + 1] << 8;
		call_bytes |= (ut32)((ut8*)buf)[bufi];
	} else {
		call_bytes = (ut32)((ut8*)buf)[bufi - align + 3] << 24;
		call_bytes |= (ut32)((ut8*)buf)[bufi - align + 2] << 16;
		call_bytes |= (ut32)((ut8*)buf)[bufi - align + 1] << 8;
		call_bytes |= (ut32)((ut8*)buf)[bufi - align];
	}
	if (call_bytes >= from && call_bytes <= to) {
		return true;
	}
	call_bytes = (ut32)((ut8*)buf)[bufi + 4] << 24;
	call_bytes |= (ut32)((ut8*)buf)[bufi + 3] << 16;
	call_bytes |= (ut32)((ut8*)buf)[bufi + 2] << 8;
	call_bytes |= (ut32)((ut8*)buf)[bufi + 1];
	call_bytes += addr + 5;
	if (call_bytes >= from && call_bytes <= to) {
		return false;
	}
	return false;
}