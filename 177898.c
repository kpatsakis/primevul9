PS_CLOSE_FUNC(files)
{
	PS_FILES_DATA;

	ps_files_close(data);

	if (data->lastkey) {
		efree(data->lastkey);
		data->lastkey = NULL;
	}

	efree(data->basedir);
	efree(data);
	*mod_data = NULL;

	return SUCCESS;
}