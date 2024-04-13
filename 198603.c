static int select_item(sc_card_t *card, const sc_path_t *path_in, sc_file_t ** file_out, int requiredType)
{
	mscfs_t *fs = MUSCLE_FS(card);
	mscfs_file_t *file_data = NULL;
	int pathlen = path_in->len;
	int r = 0;
	int objectIndex;
	u8* oid;

	mscfs_check_cache(fs);
	r = mscfs_loadFileInfo(fs, path_in->value, path_in->len, &file_data, &objectIndex);
	if(r < 0) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);

	/* Check if its the right type */
	if(requiredType >= 0 && requiredType != file_data->ef) {
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_INVALID_ARGUMENTS);
	}
	oid = file_data->objectId.id;
	/* Is it a file or directory */
	if(file_data->ef) {
		fs->currentPath[0] = oid[0];
		fs->currentPath[1] = oid[1];
		fs->currentFile[0] = oid[2];
		fs->currentFile[1] = oid[3];
	} else {
		fs->currentPath[0] = oid[pathlen - 2];
		fs->currentPath[1] = oid[pathlen - 1];
		fs->currentFile[0] = 0;
		fs->currentFile[1] = 0;
	}

	fs->currentFileIndex = objectIndex;
	if(file_out) {
		sc_file_t *file;
		file = sc_file_new();
		file->path = *path_in;
		file->size = file_data->size;
		file->id = (oid[2] << 8) | oid[3];
		if(!file_data->ef) {
			file->type = SC_FILE_TYPE_DF;
		} else {
			file->type = SC_FILE_TYPE_WORKING_EF;
			file->ef_structure = SC_FILE_EF_TRANSPARENT;
		}

		/* Setup ACLS */
		if(file_data->ef) {
			muscle_load_file_acls(file, file_data);
		} else {
			muscle_load_dir_acls(file, file_data);
			/* Setup directory acls... */
		}

		file->magic = SC_FILE_MAGIC;
		*file_out = file;
	}
	return 0;
}