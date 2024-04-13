ptp_pack_uint32_t_array(PTPParams *params, uint32_t *array, uint32_t arraylen, unsigned char **data )
{
	uint32_t i=0;

	*data = malloc ((arraylen+1)*sizeof(uint32_t));
	if (!*data)
		return 0;
	htod32a(&(*data)[0],arraylen);
	for (i=0;i<arraylen;i++)
		htod32a(&(*data)[sizeof(uint32_t)*(i+1)], array[i]);
	return (arraylen+1)*sizeof(uint32_t);
}