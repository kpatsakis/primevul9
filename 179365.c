_tiff_size (thandle_t handle)
{
	TiffBuffer *buffer = (TiffBuffer *)handle;

	return buffer->buffer_len;
}