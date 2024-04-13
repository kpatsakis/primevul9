ppt_unlzw(const char *dir, int fd, uint32_t length)
{
	int ofd;
	z_stream stream;
	unsigned char inbuff[PPT_LZW_BUFFSIZE], outbuff[PPT_LZW_BUFFSIZE];
	char fullname[NAME_MAX + 1];

	snprintf(fullname, sizeof(fullname) - 1, "%s"PATHSEP"ppt%.8lx.doc",
		dir, (long)lseek(fd, 0L, SEEK_CUR));

	ofd = open(fullname, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY|O_EXCL,
		S_IWUSR|S_IRUSR);
	if (ofd == -1) {
		cli_warnmsg("ppt_unlzw: can't create %s\n", fullname);
		return FALSE;
	}

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = (void *)NULL;
	stream.next_in = (Bytef *)inbuff;
	stream.next_out = outbuff;
	stream.avail_out = sizeof(outbuff);
	stream.avail_in = MIN(length, PPT_LZW_BUFFSIZE);

	if(cli_readn(fd, inbuff, stream.avail_in) != (int)stream.avail_in) {
		close(ofd);
		cli_unlink(fullname);
		return FALSE;
	}
	length -= stream.avail_in;

	if(inflateInit(&stream) != Z_OK) {
		close(ofd);
		cli_unlink(fullname);
		cli_warnmsg("ppt_unlzw: inflateInit failed\n");
		return FALSE;
	}

	do {
		if (stream.avail_out == 0) {
			if (cli_writen(ofd, outbuff, PPT_LZW_BUFFSIZE)
						!= PPT_LZW_BUFFSIZE) {
				close(ofd);
				inflateEnd(&stream);
				return FALSE;
			}
			stream.next_out = outbuff;
			stream.avail_out = PPT_LZW_BUFFSIZE;
		}
		if (stream.avail_in == 0) {
			stream.next_in = inbuff;
			stream.avail_in = MIN(length, PPT_LZW_BUFFSIZE);
			if (cli_readn(fd, inbuff, stream.avail_in) != (int)stream.avail_in) {
				close(ofd);
				inflateEnd(&stream);
				return FALSE;
			}
			length -= stream.avail_in;
		}
	} while(inflate(&stream, Z_NO_FLUSH) == Z_OK);

	if (cli_writen(ofd, outbuff, PPT_LZW_BUFFSIZE-stream.avail_out) != (int)(PPT_LZW_BUFFSIZE-stream.avail_out)) {
		close(ofd);
		inflateEnd(&stream);
		return FALSE;
	}
	close(ofd);
	return inflateEnd(&stream) == Z_OK;
}