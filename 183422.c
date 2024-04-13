setCurlURL(instanceData *pData, uchar **tpls)
{
	char authBuf[1024];
	uchar *searchIndex;
	uchar *searchType;
	uchar *parent;
	uchar *bulkId;
	es_str_t *url;
	int rLocal;
	int r;
	DEFiRet;

	setBaseURL(pData, &url);

	if(pData->bulkmode) {
		r = es_addBuf(&url, "_bulk", sizeof("_bulk")-1);
		parent = NULL;
	} else {
		getIndexTypeAndParent(pData, tpls, &searchIndex, &searchType, &parent, &bulkId);
		r = es_addBuf(&url, (char*)searchIndex, ustrlen(searchIndex));
		if(r == 0) r = es_addChar(&url, '/');
		if(r == 0) r = es_addBuf(&url, (char*)searchType, ustrlen(searchType));
	}
	if(r == 0) r = es_addChar(&url, '?');
	if(pData->asyncRepl) {
		if(r == 0) r = es_addBuf(&url, "replication=async&",
					sizeof("replication=async&")-1);
	}
	if(pData->timeout != NULL) {
		if(r == 0) r = es_addBuf(&url, "timeout=", sizeof("timeout=")-1);
		if(r == 0) r = es_addBuf(&url, (char*)pData->timeout, ustrlen(pData->timeout));
		if(r == 0) r = es_addChar(&url, '&');
	}
	if(parent != NULL) {
		if(r == 0) r = es_addBuf(&url, "parent=", sizeof("parent=")-1);
		if(r == 0) r = es_addBuf(&url, (char*)parent, ustrlen(parent));
	}

	free(pData->restURL);
	pData->restURL = (uchar*)es_str2cstr(url, NULL);
	curl_easy_setopt(pData->curlHandle, CURLOPT_URL, pData->restURL);
	es_deleteStr(url);
	DBGPRINTF("omelasticsearch: using REST URL: '%s'\n", pData->restURL);

	if(pData->uid != NULL) {
		rLocal = snprintf(authBuf, sizeof(authBuf), "%s:%s", pData->uid,
			         (pData->pwd == NULL) ? "" : (char*)pData->pwd);
		if(rLocal < 1) {
			errmsg.LogError(0, RS_RET_ERR, "omelasticsearch: snprintf failed "
				"when trying to build auth string (return %d)\n",
				rLocal);
			ABORT_FINALIZE(RS_RET_ERR);
		}
		curl_easy_setopt(pData->curlHandle, CURLOPT_USERPWD, authBuf);
		curl_easy_setopt(pData->curlHandle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
	}
finalize_it:
	RETiRet;
}