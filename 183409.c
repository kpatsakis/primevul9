curlResult(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	char *p = (char *)ptr;
	instanceData *pData = (instanceData*) userdata;
	char *buf;
	size_t newlen;

	newlen = pData->replyLen + size*nmemb;
	if((buf = realloc(pData->reply, newlen + 1)) == NULL) {
		DBGPRINTF("omelasticsearch: realloc failed in curlResult\n");
		return 0; /* abort due to failure */
	}
	memcpy(buf+pData->replyLen, p, size*nmemb);
	pData->replyLen = newlen;
	pData->reply = buf;
	return size*nmemb;
}