curlPost(instanceData *pData, uchar *message, int msglen, uchar **tpls)
{
	CURLcode code;
	CURL *curl = pData->curlHandle;
	DEFiRet;

	pData->reply = NULL;
	pData->replyLen = 0;

	if(pData->dynSrchIdx || pData->dynSrchType || pData->dynParent)
		CHKiRet(setCurlURL(pData, tpls));

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pData);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char *)message);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, msglen);
	code = curl_easy_perform(curl);
	switch (code) {
		case CURLE_COULDNT_RESOLVE_HOST:
		case CURLE_COULDNT_RESOLVE_PROXY:
		case CURLE_COULDNT_CONNECT:
		case CURLE_WRITE_ERROR:
			STATSCOUNTER_INC(indexConFail, mutIndexConFail);
			DBGPRINTF("omelasticsearch: we are suspending ourselfs due "
				  "to failure %lld of curl_easy_perform()\n",
				  (long long) code);
			ABORT_FINALIZE(RS_RET_SUSPENDED);
		default:
			STATSCOUNTER_INC(indexSubmit, mutIndexSubmit);
			break;
	}

	pData->reply[pData->replyLen] = '\0'; /* byte has been reserved in malloc */
	DBGPRINTF("omelasticsearch: es reply: '%s'\n", pData->reply);

	CHKiRet(checkResult(pData, message));
finalize_it:
	free(pData->reply);
	RETiRet;
}