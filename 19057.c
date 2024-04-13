static RzBinInfo *info(RzBinFile *bf) {
	rz_return_val_if_fail(bf && bf->o && bf->o->bin_obj, NULL);
	RzBinInfo *ret = RZ_NEW0(RzBinInfo);
	if (!ret) {
		return NULL;
	}
	ret->file = bf->file ? strdup(bf->file) : NULL;
	ret->type = strdup("QNX Executable");
	ret->bclass = strdup("qnx");
	ret->machine = strdup("i386");
	ret->rclass = strdup("QNX");
	ret->arch = strdup("x86");
	ret->os = strdup("any");
	ret->subsystem = strdup("any");
	ret->lang = "C/C++";
	ret->signature = true;
	return ret;
}