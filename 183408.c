checkResultBulkmode(instanceData *pData, cJSON *root)
{
	int i;
	int numitems;
	cJSON *items;
	cJSON *item;
	cJSON *create;
	cJSON *ok;
	DEFiRet;

	items = cJSON_GetObjectItem(root, "items");
	if(items == NULL || items->type != cJSON_Array) {
		DBGPRINTF("omelasticsearch: error in elasticsearch reply: "
			  "bulkmode insert does not return array, reply is: %s\n",
			  pData->reply);
		ABORT_FINALIZE(RS_RET_DATAFAIL);
	}
	numitems = cJSON_GetArraySize(items);
DBGPRINTF("omelasticsearch: %d items in reply\n", numitems);
	for(i = 0 ; i < numitems ; ++i) {
		item = cJSON_GetArrayItem(items, i);
		if(item == NULL)  {
			DBGPRINTF("omelasticsearch: error in elasticsearch reply: "
				  "cannot obtain reply array item %d\n", i);
			ABORT_FINALIZE(RS_RET_DATAFAIL);
		}
		create = cJSON_GetObjectItem(item, "create");
		if(create == NULL || create->type != cJSON_Object) {
			DBGPRINTF("omelasticsearch: error in elasticsearch reply: "
				  "cannot obtain 'create' item for #%d\n", i);
			ABORT_FINALIZE(RS_RET_DATAFAIL);
		}
		ok = cJSON_GetObjectItem(create, "ok");
		if(ok == NULL || ok->type != cJSON_True) {
			DBGPRINTF("omelasticsearch: error in elasticsearch reply: "
				  "item %d, prop ok (%p) not ok\n", i, ok);
			ABORT_FINALIZE(RS_RET_DATAFAIL);
		}
	}

finalize_it:
	RETiRet;
}