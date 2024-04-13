	if(pData->dynSrchType && pData->searchType == NULL) {
		errmsg.LogError(0, RS_RET_CONFIG_ERROR,
			"omelasticsearch: requested dynamic search type, but no "
			"name for type template given - action definition invalid");
		ABORT_FINALIZE(RS_RET_CONFIG_ERROR);
	}