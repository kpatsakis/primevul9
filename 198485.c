static int muscle_delete_file(sc_card_t *card, const sc_path_t *path_in)
{
	mscfs_t *fs = MUSCLE_FS(card);
	mscfs_file_t *file_data = NULL;
	int r = 0;

	r = mscfs_loadFileInfo(fs, path_in->value, path_in->len, &file_data, NULL);
	if(r < 0) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);
	r = muscle_delete_mscfs_file(card, file_data);
	mscfs_clear_cache(fs);
	if(r < 0) SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);
	return 0;
}