getTimeReported(msg_t *pM, enum tplFormatTypes eFmt)
{
	BEGINfunc
	if(pM == NULL)
		return "";

	switch(eFmt) {
	case tplFmtDefault:
	case tplFmtRFC3164Date:
	case tplFmtRFC3164BuggyDate:
		MsgLock(pM);
		if(pM->pszTIMESTAMP3164 == NULL) {
			pM->pszTIMESTAMP3164 = pM->pszTimestamp3164;
			datetime.formatTimestamp3164(&pM->tTIMESTAMP, pM->pszTIMESTAMP3164,
						     (eFmt == tplFmtRFC3164BuggyDate));
		}
		MsgUnlock(pM);
		return(pM->pszTIMESTAMP3164);
	case tplFmtMySQLDate:
		MsgLock(pM);
		if(pM->pszTIMESTAMP_MySQL == NULL) {
			if((pM->pszTIMESTAMP_MySQL = MALLOC(15)) == NULL) {
				MsgUnlock(pM);
				return "";
			}
			datetime.formatTimestampToMySQL(&pM->tTIMESTAMP, pM->pszTIMESTAMP_MySQL);
		}
		MsgUnlock(pM);
		return(pM->pszTIMESTAMP_MySQL);
        case tplFmtPgSQLDate:
                MsgLock(pM);
                if(pM->pszTIMESTAMP_PgSQL == NULL) {
                        if((pM->pszTIMESTAMP_PgSQL = MALLOC(21)) == NULL) {
                                MsgUnlock(pM);
                                return "";
                        }
                        datetime.formatTimestampToPgSQL(&pM->tTIMESTAMP, pM->pszTIMESTAMP_PgSQL);
                }
                MsgUnlock(pM);
                return(pM->pszTIMESTAMP_PgSQL);
	case tplFmtRFC3339Date:
		MsgLock(pM);
		if(pM->pszTIMESTAMP3339 == NULL) {
			pM->pszTIMESTAMP3339 = pM->pszTimestamp3339;
			datetime.formatTimestamp3339(&pM->tTIMESTAMP, pM->pszTIMESTAMP3339);
		}
		MsgUnlock(pM);
		return(pM->pszTIMESTAMP3339);
	case tplFmtSecFrac:
		if(pM->pszTIMESTAMP_SecFrac[0] == '\0') {
			MsgLock(pM);
			/* re-check, may have changed while we did not hold lock */
			if(pM->pszTIMESTAMP_SecFrac[0] == '\0') {
				datetime.formatTimestampSecFrac(&pM->tTIMESTAMP, pM->pszTIMESTAMP_SecFrac);
			}
			MsgUnlock(pM);
		}
		return(pM->pszTIMESTAMP_SecFrac);
	}
	ENDfunc
	return "INVALID eFmt OPTION!";
}