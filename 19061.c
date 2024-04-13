static int lmf_header_load(lmf_header *lmfh, RzBuffer *buf, Sdb *db) {
	if (rz_buf_size(buf) < sizeof(lmf_header)) {
		return false;
	}
	if (rz_buf_fread_at(buf, QNX_HEADER_ADDR, (ut8 *)lmfh, "iiiiiiiicccciiiicc", 1) < QNX_HDR_SIZE) {
		return false;
	}
	sdb_set(db, "qnx.version", sdb_fmt("0x%xH", lmfh->version), 0);
	sdb_set(db, "qnx.cflags", sdb_fmt("0x%xH", lmfh->cflags), 0);
	sdb_set(db, "qnx.cpu", sdb_fmt("0x%xH", lmfh->cpu), 0);
	sdb_set(db, "qnx.fpu", sdb_fmt("0x%xH", lmfh->fpu), 0);
	sdb_set(db, "qnx.code_index", sdb_fmt("0x%x", lmfh->code_index), 0);
	sdb_set(db, "qnx.stack_index", sdb_fmt("0x%x", lmfh->stack_index), 0);
	sdb_set(db, "qnx.heap_index", sdb_fmt("0x%x", lmfh->heap_index), 0);
	sdb_set(db, "qnx.argv_index", sdb_fmt("0x%x", lmfh->argv_index), 0);
	sdb_set(db, "qnx.code_offset", sdb_fmt("0x%x", lmfh->code_offset), 0);
	sdb_set(db, "qnx.stack_nbytes", sdb_fmt("0x%x", lmfh->stack_nbytes), 0);
	sdb_set(db, "qnx.heap_nbytes", sdb_fmt("0x%x", lmfh->heap_nbytes), 0);
	sdb_set(db, "qnx.image_base", sdb_fmt("0x%x", lmfh->image_base), 0);
	return true;
}