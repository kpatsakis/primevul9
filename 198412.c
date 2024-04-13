static int muscle_create_file(sc_card_t *card, sc_file_t *file)
{
	mscfs_t *fs = MUSCLE_FS(card);
	int objectSize = file->size;
	unsigned short read_perm = 0, write_perm = 0, delete_perm = 0;
	msc_id objectId;
	int r;
	if(file->type == SC_FILE_TYPE_DF)
		return muscle_create_directory(card, file);
	if(file->type != SC_FILE_TYPE_WORKING_EF)
		return SC_ERROR_NOT_SUPPORTED;
	if(file->id == 0) /* No null name files */
		return SC_ERROR_INVALID_ARGUMENTS;

	muscle_parse_acls(file, &read_perm, &write_perm, &delete_perm);

	mscfs_lookup_local(fs, file->id, &objectId);
	r = msc_create_object(card, objectId, objectSize, read_perm, write_perm, delete_perm);
	mscfs_clear_cache(fs);
	if(r >= 0) return 0;
	return r;
}