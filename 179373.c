_tiff_map_file (thandle_t handle,
		tdata_t  *buf,
		toff_t   *size)
{
	TiffBuffer *buffer = (TiffBuffer *)handle;

	*buf = buffer->buffer;
	*size = buffer->buffer_len;

	return 0;
}