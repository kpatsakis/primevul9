ole_copy_file_data(int s, int d, uint32_t len)
{
	unsigned char data[FILEBUFF];

	while(len > 0) {
		int todo = MIN(sizeof(data), len);

		if(cli_readn(s, data, (unsigned int)todo) != todo)
			break;
		if(cli_writen(d, data, (unsigned int)todo) != todo)
			break;
		len -= todo;
	}
}