static bool myvalid(RIO *io, ut64 addr) {
	if (addr < 0x100) {
		return false;
	}
	if (addr == UT32_MAX || addr == UT64_MAX) {	//the best of the best of the best :(
		return false;
	}
	if (!r_io_is_valid_offset (io, addr, 0)) {
		return false;
	}
	return true;
}