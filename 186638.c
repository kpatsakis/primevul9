word_read_macro_info(int fd, macro_info_t *macro_info)
{
	if(!read_uint16(fd, &macro_info->count, FALSE)) {
		cli_dbgmsg("read macro_info failed\n");
		macro_info->count = 0;
		return NULL;
	}
	cli_dbgmsg("macro count: %d\n", macro_info->count);
	if(macro_info->count == 0)
		return NULL;
	macro_info->entries = (macro_entry_t *)cli_malloc(sizeof(macro_entry_t) * macro_info->count);
	if(macro_info->entries == NULL) {
		macro_info->count = 0;
		return NULL;
	}
	if(!word_read_macro_entry(fd, macro_info)) {
		free(macro_info->entries);
		macro_info->count = 0;
		return NULL;
	}
	return macro_info;
}