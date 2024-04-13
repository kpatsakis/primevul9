static int muscle_delete_mscfs_file(sc_card_t *card, mscfs_file_t *file_data)
{
	mscfs_t *fs = MUSCLE_FS(card);
	msc_id id = file_data->objectId;
	u8* oid = id.id;
	int r;

	if(!file_data->ef) {
		int x;
		mscfs_file_t *childFile;
		/* Delete children */
		mscfs_check_cache(fs);

		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
			"DELETING Children of: %02X%02X%02X%02X\n",
			oid[0],oid[1],oid[2],oid[3]);
		for(x = 0; x < fs->cache.size; x++) {
			msc_id objectId;
			childFile = &fs->cache.array[x];
			objectId = childFile->objectId;

			if(0 == memcmp(oid + 2, objectId.id, 2)) {
				sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
					"DELETING: %02X%02X%02X%02X\n",
					objectId.id[0],objectId.id[1],
					objectId.id[2],objectId.id[3]);
				r = muscle_delete_mscfs_file(card, childFile);
				if(r < 0) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);
			}
		}
		oid[0] = oid[2];
		oid[1] = oid[3];
		oid[2] = oid[3] = 0;
		/* ??? objectId = objectId >> 16; */
	}
	if((0 == memcmp(oid, "\x3F\x00\x00\x00", 4))
		|| (0 == memcmp(oid, "\x3F\x00\x3F\x00", 4))) {
	}
	r = msc_delete_object(card, id, 1);
	/* Check if its the root... this file generally is virtual
	 * So don't return an error if it fails */
	if((0 == memcmp(oid, "\x3F\x00\x00\x00", 4))
		|| (0 == memcmp(oid, "\x3F\x00\x3F\x00", 4)))
		return 0;

	if(r < 0) {
		printf("ID: %02X%02X%02X%02X\n",
					oid[0],oid[1],oid[2],oid[3]);
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);
	}
	return 0;
}