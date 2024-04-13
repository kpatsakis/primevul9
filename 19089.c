static bool isValidAddress(RCore *core, ut64 addr) {
	// check if address is mapped
	RIOMap* map = r_io_map_get_at (core->io, addr);
	if (!map) {
		return false;
	}
	st64 fdsz = (st64)r_io_fd_size (core->io, map->fd);
	if (fdsz > 0 && map->delta > fdsz) {
		return false;
	}
	// check if associated file is opened
	RIODesc *desc = r_io_desc_get (core->io, map->fd);
	if (!desc) {
		return false;
	}
	// check if current map->fd is null://
	if (!strncmp (desc->name, "null://", 7)) {
		return false;
	}
	return true;
}