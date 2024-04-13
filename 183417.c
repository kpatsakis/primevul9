	if (pData->curlHandle) {
		curl_easy_cleanup(pData->curlHandle);
		pData->curlHandle = NULL;
	}