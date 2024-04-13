checkConn(instanceData *pData)
{
	es_str_t *url;
	CURL *curl = NULL;
	CURLcode res;
	char *cstr;
	DEFiRet;

	setBaseURL(pData, &url);
	curl = curl_easy_init();
	if(curl == NULL) {
		DBGPRINTF("omelasticsearch: checkConn() curl_easy_init() failed\n");
		ABORT_FINALIZE(RS_RET_SUSPENDED);
	}
	cstr = es_str2cstr(url, NULL);
	curl_easy_setopt(curl, CURLOPT_URL, cstr);
	free(cstr);

	pData->reply = NULL;
	pData->replyLen = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pData);
	res = curl_easy_perform(curl);
	if(res != CURLE_OK) {
		DBGPRINTF("omelasticsearch: checkConn() curl_easy_perform() "
			  "failed: %s\n", curl_easy_strerror(res));
		ABORT_FINALIZE(RS_RET_SUSPENDED);
	}
	free(pData->reply);
	DBGPRINTF("omelasticsearch: checkConn() completed with success\n");

finalize_it:
	if(curl != NULL)
		curl_easy_cleanup(curl);
	RETiRet;
}