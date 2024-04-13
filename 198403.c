static int _listFile(mscfs_file_t *file, int reset, void *udata)
{
	int next = reset ? 0x00 : 0x01;
	return msc_list_objects( (sc_card_t*)udata, next, file);
}