static int muscle_read_binary(sc_card_t *card, unsigned int idx, u8* buf, size_t count, unsigned long flags)
{
	mscfs_t *fs = MUSCLE_FS(card);
	int r;
	msc_id objectId;
	u8* oid = objectId.id;
	mscfs_file_t *file;

	r = mscfs_check_selection(fs, -1);
	if(r < 0) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
	file = &fs->cache.array[fs->currentFileIndex];
	objectId = file->objectId;
	/* memcpy(objectId.id, file->objectId.id, 4); */
	if(!file->ef) {
		oid[0] = oid[2];
		oid[1] = oid[3];
		oid[2] = oid[3] = 0;
	}
	r = msc_read_object(card, objectId, idx, buf, count);
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
}