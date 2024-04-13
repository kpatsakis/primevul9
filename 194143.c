static inline char *getTimeGenerated(msg_t *pM, enum tplFormatTypes eFmt)
{
	BEGINfunc
	if(pM == NULL)
		return "";

	switch(eFmt) {
	case tplFmtDefault:
		MsgLock(pM);
		if(pM->pszRcvdAt3164 == NULL) {
			if((pM->pszRcvdAt3164 = MALLOC(16)) == NULL) {
				MsgUnlock(pM);
				return "";
			}
			datetime.formatTimestamp3164(&pM->tRcvdAt, pM->pszRcvdAt3164, 0);
		}
		MsgUnlock(pM);
		return(pM->pszRcvdAt3164);
	case tplFmtMySQLDate:
		MsgLock(pM);
		if(pM->pszRcvdAt_MySQL == NULL) {
			if((pM->pszRcvdAt_MySQL = MALLOC(15)) == NULL) {
				MsgUnlock(pM);
				return "";
			}
			datetime.formatTimestampToMySQL(&pM->tRcvdAt, pM->pszRcvdAt_MySQL);
		}
		MsgUnlock(pM);
		return(pM->pszRcvdAt_MySQL);
        case tplFmtPgSQLDate:
                MsgLock(pM);
                if(pM->pszRcvdAt_PgSQL == NULL) {
                        if((pM->pszRcvdAt_PgSQL = MALLOC(21)) == NULL) {
                                MsgUnlock(pM);
                                return "";
                        }
                        datetime.formatTimestampToPgSQL(&pM->tRcvdAt, pM->pszRcvdAt_PgSQL);
                }
                MsgUnlock(pM);
                return(pM->pszRcvdAt_PgSQL);
	case tplFmtRFC3164Date:
	case tplFmtRFC3164BuggyDate:
		MsgLock(pM);
		if(pM->pszRcvdAt3164 == NULL) {
			if((pM->pszRcvdAt3164 = MALLOC(16)) == NULL) {
					MsgUnlock(pM);
					return "";
				}
			datetime.formatTimestamp3164(&pM->tRcvdAt, pM->pszRcvdAt3164,
						     (eFmt == tplFmtRFC3164BuggyDate));
		}
		MsgUnlock(pM);
		return(pM->pszRcvdAt3164);
	case tplFmtRFC3339Date:
		MsgLock(pM);
		if(pM->pszRcvdAt3339 == NULL) {
			if((pM->pszRcvdAt3339 = MALLOC(33)) == NULL) {
				MsgUnlock(pM);
				return "";
			}
			datetime.formatTimestamp3339(&pM->tRcvdAt, pM->pszRcvdAt3339);
		}
		MsgUnlock(pM);
		return(pM->pszRcvdAt3339);
	case tplFmtSecFrac:
		if(pM->pszRcvdAt_SecFrac[0] == '\0') {
			MsgLock(pM);
			/* re-check, may have changed while we did not hold lock */
			if(pM->pszRcvdAt_SecFrac[0] == '\0') {
				datetime.formatTimestampSecFrac(&pM->tRcvdAt, pM->pszRcvdAt_SecFrac);
			}
			MsgUnlock(pM);
		}
		return(pM->pszRcvdAt_SecFrac);
	}
	ENDfunc
	return "INVALID eFmt OPTION!";
}