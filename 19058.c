static ut64 baddr(RzBinFile *bf) {
	QnxObj *qo = bf->o->bin_obj;
	return qo ? qo->lmfh.image_base : 0;
}