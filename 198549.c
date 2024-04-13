static int muscle_select_file(sc_card_t *card, const sc_path_t *path_in,
			     sc_file_t **file_out)
{
	int r;

	assert(card != NULL && path_in != NULL);

	switch (path_in->type) {
	case SC_PATH_TYPE_FILE_ID:
		r = select_item(card, path_in, file_out, 1);
		break;
	case SC_PATH_TYPE_DF_NAME:
		r = select_item(card, path_in, file_out, 0);
		break;
	case SC_PATH_TYPE_PATH:
		r = select_item(card, path_in, file_out, -1);
		break;
	default:
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, SC_ERROR_INVALID_ARGUMENTS);
	}
	if(r > 0) r = 0;
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE,r);
}