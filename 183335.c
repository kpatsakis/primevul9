ptp_get_packed_stringcopy(PTPParams *params, char *string, uint32_t *packed_size)
{
	uint8_t packed[PTP_MAXSTRLEN*2+3], len;
	size_t plen;
	unsigned char *retcopy = NULL;
  
	if (string == NULL)
	  ptp_pack_string(params, "", (unsigned char*) packed, 0, &len);
	else
	  ptp_pack_string(params, string, (unsigned char*) packed, 0, &len);
  
	/* returned length is in characters, then one byte for string length */
	plen = len*2 + 1;
	
	retcopy = malloc(plen);
	if (!retcopy) {
		*packed_size = 0;
		return NULL;
	}
	memcpy(retcopy, packed, plen);
	*packed_size = plen;
	return (retcopy);
}