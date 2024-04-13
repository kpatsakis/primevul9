static int muscle_create_directory(sc_card_t *card, sc_file_t *file)
{
	mscfs_t *fs = MUSCLE_FS(card);
	msc_id objectId;
	u8* oid = objectId.id;
	unsigned id = file->id;
	unsigned short read_perm = 0, write_perm = 0, delete_perm = 0;
	int objectSize;
	int r;
	if(id == 0) /* No null name files */
		return SC_ERROR_INVALID_ARGUMENTS;

	/* No nesting directories */
	if(fs->currentPath[0] != 0x3F || fs->currentPath[1] != 0x00)
		return SC_ERROR_NOT_SUPPORTED;
	oid[0] = ((id & 0xFF00) >> 8) & 0xFF;
	oid[1] = id & 0xFF;
	oid[2] = oid[3] = 0;

	objectSize = file->size;

	muscle_parse_acls(file, &read_perm, &write_perm, &delete_perm);
	r = msc_create_object(card, objectId, objectSize, read_perm, write_perm, delete_perm);
	mscfs_clear_cache(fs);
	if(r >= 0) return 0;
	return r;
}