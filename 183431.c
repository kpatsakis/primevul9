buildBatch(instanceData *pData, uchar *message, uchar **tpls)
{
	int length = strlen((char *)message);
	int r;
	uchar *searchIndex;
	uchar *searchType;
	uchar *parent;
	uchar *bulkId = NULL;
	DEFiRet;
#	define META_STRT "{\"index\":{\"_index\": \""
#	define META_TYPE "\",\"_type\":\""
#	define META_PARENT "\",\"_parent\":\""
#   define META_ID "\", \"_id\":\""
#	define META_END  "\"}}\n"

	getIndexTypeAndParent(pData, tpls, &searchIndex, &searchType, &parent, &bulkId);
	r = es_addBuf(&pData->batch.data, META_STRT, sizeof(META_STRT)-1);
	if(r == 0) r = es_addBuf(&pData->batch.data, (char*)searchIndex,
				 ustrlen(searchIndex));
	if(r == 0) r = es_addBuf(&pData->batch.data, META_TYPE, sizeof(META_TYPE)-1);
	if(r == 0) r = es_addBuf(&pData->batch.data, (char*)searchType,
				 ustrlen(searchType));
	if(parent != NULL) {
		if(r == 0) r = es_addBuf(&pData->batch.data, META_PARENT, sizeof(META_PARENT)-1);
		if(r == 0) r = es_addBuf(&pData->batch.data, (char*)parent, ustrlen(parent));
	}
	if(bulkId != NULL) {
		if(r == 0) r = es_addBuf(&pData->batch.data, META_ID, sizeof(META_ID)-1);
		if(r == 0) r = es_addBuf(&pData->batch.data, (char*)bulkId, ustrlen(bulkId));
	}
	if(r == 0) r = es_addBuf(&pData->batch.data, META_END, sizeof(META_END)-1);
	if(r == 0) r = es_addBuf(&pData->batch.data, (char*)message, length);
	if(r == 0) r = es_addBuf(&pData->batch.data, "\n", sizeof("\n")-1);
	if(r != 0) {
		DBGPRINTF("omelasticsearch: growing batch failed with code %d\n", r);
		ABORT_FINALIZE(RS_RET_ERR);
	}
	iRet = RS_RET_DEFER_COMMIT;

finalize_it:
	RETiRet;
}