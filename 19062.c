static char *signature(RzBinFile *bf, bool json) {
	char buf[64];
	QnxObj *qo = bf->o->bin_obj;
	if (!qo) {
		return NULL;
	}
	if (json) {
		PJ *pj = pj_new();
		pj_n(pj, qo->rwend.signature);
		return pj_drain(pj);
	} else {
		return rz_str_dup(NULL, sdb_itoa(qo->rwend.signature, buf, 10));
	}
}