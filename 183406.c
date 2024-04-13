	if(pData->dynSrchIdx && pData->searchIndex == NULL) {
		errmsg.LogError(0, RS_RET_CONFIG_ERROR,
			"omelasticsearch: requested dynamic search index, but no "
			"name for index template given - action definition invalid");
		ABORT_FINALIZE(RS_RET_CONFIG_ERROR);
	}