static bool stringAt(RCore *core, ut64 addr) {
	ut8 buf[32];
	r_io_read_at (core->io, addr - 1, buf, sizeof (buf));
	// check if previous byte is a null byte, all strings, except pascal ones should be like this
	if (buf[0] != 0) {
		return false;
	}
	return is_string (buf + 1, 31, NULL);
}