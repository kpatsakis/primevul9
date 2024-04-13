static int is_seq(unsigned char * seq, unsigned int *seq_size, unsigned int *seq_len) 
{
	int i,j,k;

	if (seq[0] != 0x30)
		return 0;   /* not a sequence */
	if (seq[1] & 0x80) {
		i = seq[1] & 0x7f;
		if (i > 2 || i == 0) 
			return 0; /* cert would be bigger then 65k or zero */
		if (seq[2] == 0) 
			return 0; /* DER would not have extra zero */		
		k = 0;
		for (j = 0; j < i; j++) {
			k = (k << 8) + seq[j + 2];
		}
		if (k < 128)
			return 0; /* DER would have used single byte for len */
	} else {
		  i = 0;
		  k = seq[1];
	}
	
	*seq_size = i + 2;
	*seq_len = k;
	return 1;
}