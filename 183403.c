setInstParamDefaults(instanceData *pData)
{
	pData->server = NULL;
	pData->port = 9200;
	pData->uid = NULL;
	pData->pwd = NULL;
	pData->searchIndex = NULL;
	pData->searchType = NULL;
	pData->parent = NULL;
	pData->timeout = NULL;
	pData->dynSrchIdx = 0;
	pData->dynSrchType = 0;
	pData->dynParent = 0;
	pData->asyncRepl = 0;
	pData->bulkmode = 0;
	pData->tplName = NULL;
	pData->errorFile = NULL;
	pData->dynBulkId= 0;
	pData->bulkId = NULL;
}