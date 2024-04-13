int srv_set_message(char *buf,
                        int num_words,
                        int num_bytes,
                        bool zero)
{
	if (zero && (num_words || num_bytes)) {
		memset(buf + smb_size,'\0',num_words*2 + num_bytes);
	}
	SCVAL(buf,smb_wct,num_words);
	SSVAL(buf,smb_vwv + num_words*SIZEOFWORD,num_bytes);
	smb_setlen(buf,(smb_size + num_words*2 + num_bytes - 4));
	return (smb_size + num_words*2 + num_bytes);
}