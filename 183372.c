ucs2strlen(uint16_t const * const unicstr)
{
	int length = 0;
	
	/* Unicode strings are terminated with 2 * 0x00 */
	for(length = 0; unicstr[length] != 0x0000U; length ++);
	return length;
}