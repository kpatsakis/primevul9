static Sdb *get_sdb(RzBinFile *bf) {
	RzBinObject *o = bf->o;
	if (!o) {
		return NULL;
	}
	QnxObj *qo = o->bin_obj;
	return qo ? qo->kv : NULL;
}