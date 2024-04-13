magic_buffer(struct magic_set *ms, const void *buf, size_t nb)
{
	if (ms == NULL)
		return NULL;
	if (file_reset(ms) == -1)
		return NULL;
	/*
	 * The main work is done here!
	 * We have the file name and/or the data buffer to be identified. 
	 */
	if (file_buffer(ms, NULL, NULL, buf, nb) == -1) {
		return NULL;
	}
	return file_getbuffer(ms);
}