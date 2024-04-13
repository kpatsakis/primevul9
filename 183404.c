	if(pData->bulkmode) {
		pData->batch.currTpl1 = NULL;
		pData->batch.currTpl2 = NULL;
		if((pData->batch.data = es_newStr(1024)) == NULL) {
			DBGPRINTF("omelasticsearch: error creating batch string "
			          "turned off bulk mode\n");
			pData->bulkmode = 0; /* at least it works */
		}
	}