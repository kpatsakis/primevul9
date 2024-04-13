static int cac_path_from_cardurl(sc_card_t *card, sc_path_t *path, cac_card_url_t *val, int len)
{
	if (len < 10) {
		return SC_ERROR_INVALID_DATA;
	}
	sc_mem_clear(path, sizeof(sc_path_t));
	memcpy(path->aid.value, &val->rid, sizeof(val->rid));
	memcpy(&path->aid.value[5], &val->applicationID, sizeof(val->applicationID));
	path->aid.len = sizeof(val->rid) + sizeof(val->applicationID);
	memcpy(path->value, &val->objectID, sizeof(val->objectID));
	path->len = sizeof(val->objectID);
	path->type = SC_PATH_TYPE_FILE_ID;
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
		 "path->aid=%x %x %x %x %x %x %x  len=%"SC_FORMAT_LEN_SIZE_T"u, path->value = %x %x len=%"SC_FORMAT_LEN_SIZE_T"u path->type=%d (%x)",
		 path->aid.value[0], path->aid.value[1], path->aid.value[2],
		 path->aid.value[3], path->aid.value[4], path->aid.value[5],
		 path->aid.value[6], path->aid.len, path->value[0],
		 path->value[1], path->len, path->type, path->type);
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,
		 "rid=%x %x %x %x %x  len=%"SC_FORMAT_LEN_SIZE_T"u appid= %x %x len=%"SC_FORMAT_LEN_SIZE_T"u objid= %x %x len=%"SC_FORMAT_LEN_SIZE_T"u",
		 val->rid[0], val->rid[1], val->rid[2], val->rid[3],
		 val->rid[4], sizeof(val->rid), val->applicationID[0],
		 val->applicationID[1], sizeof(val->applicationID),
		 val->objectID[0], val->objectID[1], sizeof(val->objectID));

	return SC_SUCCESS;
}