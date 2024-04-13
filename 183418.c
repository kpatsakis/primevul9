setBaseURL(instanceData *pData, es_str_t **url)
{
	char portBuf[64];
	int r;
	DEFiRet;

	*url = es_newStr(128);
	snprintf(portBuf, sizeof(portBuf), "%d", pData->port);
	r = es_addBuf(url, "http://", sizeof("http://")-1);
	if(r == 0) r = es_addBuf(url, (char*)pData->server, strlen((char*)pData->server));
	if(r == 0) r = es_addChar(url, ':');
	if(r == 0) r = es_addBuf(url, portBuf, strlen(portBuf));
	if(r == 0) r = es_addChar(url, '/');
	RETiRet;
}