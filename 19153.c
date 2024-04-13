static bool esilbreak_mem_read(RAnalEsil *esil, ut64 addr, ut8 *buf, int len) {
	ut8 str[128];
	if (addr != UT64_MAX) {
		esilbreak_last_read = addr;
	}
	handle_var_stack_access (esil, addr, R_ANAL_VAR_ACCESS_TYPE_READ, len);
	if (myvalid (mycore->io, addr) && r_io_read_at (mycore->io, addr, (ut8*)buf, len)) {
		ut64 refptr;
		bool trace = true;
		switch (len) {
		case 2:
			esilbreak_last_data = refptr = (ut64)r_read_ble16 (buf, esil->anal->big_endian);
			break;
		case 4:
			esilbreak_last_data = refptr = (ut64)r_read_ble32 (buf, esil->anal->big_endian);
			break;
		case 8:
			esilbreak_last_data = refptr = r_read_ble64 (buf, esil->anal->big_endian);
			break;
		default:
			trace = false;
			r_io_read_at (mycore->io, addr, (ut8*)buf, len);
			break;
		}
		// TODO incorrect
		bool validRef = false;
		if (trace && myvalid (mycore->io, refptr)) {
			if (ntarget == UT64_MAX || ntarget == refptr) {
				str[0] = 0;
				if (r_io_read_at (mycore->io, refptr, str, sizeof (str)) < 1) {
					//eprintf ("Invalid read\n");
					str[0] = 0;
					validRef = false;
				} else {
					r_anal_xrefs_set (mycore->anal, esil->address, refptr, R_ANAL_REF_TYPE_DATA);
					str[sizeof (str) - 1] = 0;
					add_string_ref (mycore, esil->address, refptr);
					esilbreak_last_data = UT64_MAX;
					validRef = true;
				}
			}
		}

		/** resolve ptr */
		if (ntarget == UT64_MAX || ntarget == addr || (ntarget == UT64_MAX && !validRef)) {
			r_anal_xrefs_set (mycore->anal, esil->address, addr, R_ANAL_REF_TYPE_DATA);
		}
	}
	return false; // fallback
}