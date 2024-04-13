uchar *fill(Scanner *s, uchar *cursor){
	if(!s->eof){
		unsigned int cnt = s->tok - s->bot;
		if(cnt){
			memcpy(s->bot, s->tok, s->lim - s->tok);
			s->tok = s->bot;
			s->ptr -= cnt;
			cursor -= cnt;
			s->pos -= cnt;
			s->lim -= cnt;
		}
		if((s->top - s->lim) < BSIZE){
			uchar *buf = (uchar*) timelib_malloc(((s->lim - s->bot) + BSIZE)*sizeof(uchar));
			memcpy(buf, s->tok, s->lim - s->tok);
			s->tok = buf;
			s->ptr = &buf[s->ptr - s->bot];
			cursor = &buf[cursor - s->bot];
			s->pos = &buf[s->pos - s->bot];
			s->lim = &buf[s->lim - s->bot];
			s->top = &s->lim[BSIZE];
			timelib_free(s->bot);
			s->bot = buf;
		}
		if((cnt = read(s->fd, (char*) s->lim, BSIZE)) != BSIZE){
			s->eof = &s->lim[cnt]; *(s->eof)++ = '\n';
		}
		s->lim += cnt;
	}
	return cursor;
}