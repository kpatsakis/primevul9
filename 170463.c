static ut64 pa2va(RzBinFile *bf, ut64 offset) {
	rz_return_val_if_fail(bf && bf->rbin, offset);
	RzIO *io = bf->rbin->iob.io;
	if (!io || !io->va) {
		return offset;
	}
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	return bin ? MACH0_(paddr_to_vaddr)(bin, offset) : offset;
}