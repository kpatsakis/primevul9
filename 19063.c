static void header(RzBinFile *bf) {
	rz_return_if_fail(bf && bf->o && bf->rbin);
	QnxObj *bin = bf->o->bin_obj;
	RzBin *rbin = bf->rbin;
	rbin->cb_printf("QNX file header:\n");
	rbin->cb_printf("version : 0x%xH\n", bin->lmfh.version);
	rbin->cb_printf("cflags : 0x%xH\n", bin->lmfh.cflags);
	rbin->cb_printf("cpu : 0x%xH\n", bin->lmfh.cpu);
	rbin->cb_printf("fpu : 0x%xH\n", bin->lmfh.fpu);
	rbin->cb_printf("code_index : 0x%xH\n", bin->lmfh.code_index);
	rbin->cb_printf("stack_index : 0x%xH\n", bin->lmfh.stack_index);
	rbin->cb_printf("heap_index : 0x%xH\n", bin->lmfh.heap_index);
	rbin->cb_printf("argv_index : 0x%xH\n", bin->lmfh.argv_index);
	rbin->cb_printf("spare2[4] : 0x0H\n");
	rbin->cb_printf("code_offset : 0x%xH\n", bin->lmfh.code_offset);
	rbin->cb_printf("stack_nbytes : 0x%xH\n", bin->lmfh.stack_nbytes);
	rbin->cb_printf("heap_nbytes : 0x%xH\n", bin->lmfh.heap_nbytes);
	rbin->cb_printf("image_base : 0x%xH\n", bin->lmfh.image_base);
	rbin->cb_printf("spare3[2] : 0x0H\n");
}