struct MACH0_(obj_t) * MACH0_(new_buf)(RzBuffer *buf, struct MACH0_(opts_t) * options) {
	rz_return_val_if_fail(buf, NULL);
	struct MACH0_(obj_t) *bin = RZ_NEW0(struct MACH0_(obj_t));
	if (bin) {
		bin->b = rz_buf_ref(buf);
		bin->main_addr = UT64_MAX;
		bin->kv = sdb_new(NULL, "bin.mach0", 0);
		bin->hash = rz_hash_new();
		bin->size = rz_buf_size(bin->b);
		if (options) {
			bin->options = *options;
		}
		if (!init(bin)) {
			return MACH0_(mach0_free)(bin);
		}
	}
	return bin;
}