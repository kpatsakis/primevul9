	if (pData->postHeader) {
		curl_slist_free_all(pData->postHeader);
		pData->postHeader = NULL;
	}