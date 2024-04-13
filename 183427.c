curlSetup(instanceData *pData)
{
	HEADER *header;
	CURL *handle;

	handle = curl_easy_init();
	if (handle == NULL) {
		return RS_RET_OBJ_CREATION_FAILED;
	}

	header = curl_slist_append(NULL, "Content-Type: text/json; charset=utf-8");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);

	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curlResult);
	curl_easy_setopt(handle, CURLOPT_POST, 1);

	pData->curlHandle = handle;
	pData->postHeader = header;

	if(    pData->bulkmode
	   || (pData->dynSrchIdx == 0 && pData->dynSrchType == 0 && pData->dynParent == 0)) {
		/* in this case, we know no tpls are involved in the request-->NULL OK! */
		setCurlURL(pData, NULL);
	}

	if(Debug) {
		if(pData->dynSrchIdx == 0 && pData->dynSrchType == 0 && pData->dynParent == 0)
			dbgprintf("omelasticsearch setup, using static REST URL\n");
		else
			dbgprintf("omelasticsearch setup, we have a dynamic REST URL\n");
	}
	return RS_RET_OK;
}