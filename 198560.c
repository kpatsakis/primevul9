void decode_version(unsigned char *bcd, unsigned int *major, unsigned int *minor, unsigned int *fix)
{
	*major = 0;
	*minor = 0;
	*fix = 0;

	/* decode BCD to decimal */
	if ((bcd[0]>>4) < 10 && ((bcd[0]&0xF) < 10) && ((bcd[1]>>4) < 10)) {
		*major = (bcd[0]>>4)*100 + (bcd[0]&0xF)*10 + (bcd[1]>>4);
	}
	if (((bcd[1]&0xF) < 10) && ((bcd[2]>>4) < 10) && ((bcd[2]&0xF) < 10)) {
		*minor = (bcd[1]&0xF)*100 + (bcd[2]>>4)*10 + (bcd[2]&0xF);
	}
	if ((bcd[3]>>4) < 10 && ((bcd[3]&0xF) < 10)
			&& (bcd[4]>>4) < 10 && ((bcd[4]&0xF) < 10)) {
		*fix = (bcd[3]>>4)*1000 + (bcd[3]&0xF)*100
			+ (bcd[4]>>4)*10 + (bcd[4]&0xF);
	}
}