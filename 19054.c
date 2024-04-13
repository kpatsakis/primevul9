static void destroy(RzBinFile *bf) {
	QnxObj *qo = bf->o->bin_obj;
	rz_list_free(qo->sections);
	rz_list_free(qo->maps);
	rz_list_free(qo->fixups);
	bf->o->bin_obj = NULL;
	free(qo);
}