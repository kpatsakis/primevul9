static int muscle_update_binary(sc_card_t *card, unsigned int idx, const u8* buf, size_t count, unsigned long flags)
{
	mscfs_t *fs = MUSCLE_FS(card);
	int r;
	mscfs_file_t *file;
	msc_id objectId;
	u8* oid = objectId.id;

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
	if(file->size < idx + count) {
		int newFileSize = idx + count;
		u8* buffer = malloc(newFileSize);
		if(buffer == NULL) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_OUT_OF_MEMORY);

		r = msc_read_object(card, objectId, 0, buffer, file->size);
		/* TODO: RETRIEVE ACLS */
		if(r < 0) goto update_bin_free_buffer;
		r = msc_delete_object(card, objectId, 0);
		if(r < 0) goto update_bin_free_buffer;
		r = msc_create_object(card, objectId, newFileSize, 0,0,0);
		if(r < 0) goto update_bin_free_buffer;
		memcpy(buffer + idx, buf, count);
		r = msc_update_object(card, objectId, 0, buffer, newFileSize);
		if(r < 0) goto update_bin_free_buffer;
		file->size = newFileSize;
update_bin_free_buffer:
		free(buffer);
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
	} else {
		r = msc_update_object(card, objectId, idx, buf, count);
	}
	/* mscfs_clear_cache(fs); */
	return r;
}