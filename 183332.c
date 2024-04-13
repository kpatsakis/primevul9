ptp_unpack_uint16_t_array(PTPParams *params, unsigned char* data, unsigned int offset, unsigned int datalen, uint16_t **array)
{
	uint32_t n, i=0;

	if (!data)
		return 0;
	*array = NULL;
	n=dtoh32a(&data[offset]);
	if (n >= UINT_MAX/sizeof(uint16_t))
		return 0;
	if (!n)
		return 0;
	if (offset + sizeof(uint32_t) > datalen)
		return 0;
	if (offset + sizeof(uint32_t)+sizeof(uint16_t)*n > datalen) {
		ptp_debug (params ,"array runs over datalen bufferend (%d vs %d)", offset + sizeof(uint32_t)+n*sizeof(uint16_t) , datalen);
		return 0;
	}
	*array = malloc (n*sizeof(uint16_t));
	if (!*array)
		return 0;
	for (i=0;i<n;i++)
		(*array)[i]=dtoh16a(&data[offset+(sizeof(uint16_t)*(i+2))]);
	return n;
}