getIndexTypeAndParent(instanceData *pData, uchar **tpls,
		      uchar **srchIndex, uchar **srchType, uchar **parent,
			  uchar **bulkId)
{
	if(pData->dynSrchIdx) {
		*srchIndex = tpls[1];
		if(pData->dynSrchType) {
			*srchType = tpls[2];
			if(pData->dynParent) {
				*parent = tpls[3];
				if(pData->dynBulkId) {
					*bulkId = tpls[4];
				}
			} else {
				*parent = pData->parent;
				if(pData->dynBulkId) {
					*bulkId = tpls[3];
				}
			}
		} else  {
			*srchType = pData->searchType;
			if(pData->dynParent) {
				*parent = tpls[2];
				if(pData->dynBulkId) {
					*bulkId = tpls[3];
				}
			} else {
				*parent = pData->parent;
				if(pData->dynBulkId) {
					*bulkId = tpls[2];
				}
			}
		}
	} else {
		*srchIndex = pData->searchIndex;
		if(pData->dynSrchType) {
			*srchType = tpls[1];
			if(pData->dynParent) {
				*parent = tpls[2];
                 if(pData->dynBulkId) {
                    *bulkId = tpls[3];
                }
			} else {
				*parent = pData->parent;
                if(pData->dynBulkId) {
                    *bulkId = tpls[2];
                }
			}
		} else  {
			*srchType = pData->searchType;
			if(pData->dynParent) {
				*parent = tpls[1];
                if(pData->dynBulkId) {
                    *bulkId = tpls[2];
                }
			} else {
				*parent = pData->parent;
                if(pData->dynBulkId) {
                    *bulkId = tpls[1];
                }
			}
		}
	}
}