int expread(off_t a, char *buf, size_t len, CLIENT *client) {
	off_t rdlen, offset;
	off_t mapcnt, mapl, maph, pagestart;

	if (!(client->server->flags & F_COPYONWRITE))
		return rawexpread(a, buf, len, client);
	DEBUG3("Asked to read %d bytes at %Lu.\n", len, (unsigned long long)a);

	mapl=a/DIFFPAGESIZE; maph=(a+len-1)/DIFFPAGESIZE;

	for (mapcnt=mapl;mapcnt<=maph;mapcnt++) {
		pagestart=mapcnt*DIFFPAGESIZE;
		offset=a-pagestart;
		rdlen=(0<DIFFPAGESIZE-offset && len<(size_t)(DIFFPAGESIZE-offset)) ?
			len : (size_t)DIFFPAGESIZE-offset;
		if (client->difmap[mapcnt]!=(u32)(-1)) { /* the block is already there */
			DEBUG3("Page %Lu is at %lu\n", (unsigned long long)mapcnt,
			       (unsigned long)(client->difmap[mapcnt]));
			myseek(client->difffile, client->difmap[mapcnt]*DIFFPAGESIZE+offset);
			if (read(client->difffile, buf, rdlen) != rdlen) return -1;
		} else { /* the block is not there */
			DEBUG2("Page %Lu is not here, we read the original one\n",
			       (unsigned long long)mapcnt);
			if(rawexpread(a, buf, rdlen, client)) return -1;
		}
		len-=rdlen; a+=rdlen; buf+=rdlen;
	}
	return 0;
}