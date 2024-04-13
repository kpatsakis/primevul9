cli_vba_inflate(int fd, off_t offset, int *size)
{
	unsigned int pos, shift, mask, distance, clean;
	uint8_t flag;
	uint16_t token;
	blob *b;
	unsigned char buffer[VBA_COMPRESSION_WINDOW];

	if(fd < 0)
		return NULL;

	b = blobCreate();

	if(b == NULL)
		return NULL;

	lseek(fd, offset+3, SEEK_SET); /* 1byte ?? , 2byte length ?? */
	clean = TRUE;
	pos = 0;

	while (cli_readn(fd, &flag, 1) == 1) {
		for(mask = 1; mask < 0x100; mask<<=1) {
			unsigned int winpos = pos % VBA_COMPRESSION_WINDOW;
			if (flag & mask) {
				uint16_t len;
				unsigned int srcpos;

				if(!read_uint16(fd, &token, FALSE)) {
					blobDestroy(b);
					if(size)
						*size = 0;
					return NULL;
				}
				shift = 12 - (winpos > 0x10)
						- (winpos > 0x20)
						- (winpos > 0x40)
						- (winpos > 0x80)
						- (winpos > 0x100)
						- (winpos > 0x200)
						- (winpos > 0x400)
						- (winpos > 0x800);
				len = (uint16_t)((token & ((1 << shift) - 1)) + 3);
				distance = token >> shift;

				srcpos = pos - distance - 1;
				if((((srcpos + len) % VBA_COMPRESSION_WINDOW) < winpos) &&
				   ((winpos + len) < VBA_COMPRESSION_WINDOW) &&
				   (((srcpos % VBA_COMPRESSION_WINDOW) + len) < VBA_COMPRESSION_WINDOW) &&
				   (len <= VBA_COMPRESSION_WINDOW)) {
					srcpos %= VBA_COMPRESSION_WINDOW;
					memcpy(&buffer[winpos], &buffer[srcpos],
						len);
					pos += len;
				} else
					while(len-- > 0) {
						srcpos = (pos - distance - 1) % VBA_COMPRESSION_WINDOW;
						buffer[pos++ % VBA_COMPRESSION_WINDOW] = buffer[srcpos];
					}
			} else {
				if((pos != 0) && (winpos == 0) && clean) {
					if (cli_readn(fd, &token, 2) != 2) {
						blobDestroy(b);
						if(size)
							*size = 0;
						return NULL;
					}
					(void)blobAddData(b, buffer, VBA_COMPRESSION_WINDOW);
					clean = FALSE;
					break;
				}
				if(cli_readn(fd, &buffer[winpos], 1) == 1)
					pos++;
			}
			clean = TRUE;
		}
	}

	if(blobAddData(b, buffer, pos%VBA_COMPRESSION_WINDOW) < 0) {
		blobDestroy(b);
		if(size)
			*size = 0;
		return NULL;
	}

	if(size)
		*size = (int)blobGetDataSize(b);
	return (unsigned char *)blobToMem(b);
}