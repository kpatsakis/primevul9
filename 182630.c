static char *uue(void *in)
{
	int i;
	static unsigned char res[7];
	unsigned char *data = (unsigned char *)in;

	res[0] = ENC( data[0] >> 2 );
	res[1] = ENC( ((data[0] << 4) & 060) + ((data[1] >> 4) & 017) );
	res[2] = ENC( ((data[1] << 2) & 074) + ((data[2] >> 6) & 03) );
	res[3] = ENC( data[2] & 077 );

	res[4] = ENC( data[3] >> 2 );
	res[5] = ENC( (data[3] << 4) & 060 );
	res[6] = 0;

	for(i = 0; i < 6; i++) {
		if (res[i] == ' ') res[i] = '`';
		if (res[i] < 32 || res[i] > 127)
			printf("uue: protocol error ?!\n");
	}
	return (char *)res;
}