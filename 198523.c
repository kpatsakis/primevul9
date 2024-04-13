static int cac_fill_object_info(list_t *list, cac_object_t **entry, sc_pkcs15_data_info_t *obj_info)
{
	memset(obj_info, 0, sizeof(sc_pkcs15_data_info_t));
	if (*entry == NULL) {
		return SC_ERROR_FILE_END_REACHED;
	}

	obj_info->path = (*entry)->path;
	obj_info->path.count = CAC_MAX_SIZE-1; /* read something from the object */
	obj_info->id.value[0] = ((*entry)->fd >> 8) & 0xff;
	obj_info->id.value[1] = (*entry)->fd & 0xff;
	obj_info->id.len = 2;
	strncpy(obj_info->app_label, (*entry)->name, SC_PKCS15_MAX_LABEL_SIZE-1);
	*entry = list_iterator_next(list);
	return SC_SUCCESS;
}