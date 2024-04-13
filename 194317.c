rsRetVal rsCStrSzStrMatchRegex(cstr_t *pCS1, uchar *psz, int iType, void *rc)
{
	regex_t **cache = (regex_t**) rc;
	int ret;
	DEFiRet;

	assert(pCS1 != NULL);
	assert(psz != NULL);
	assert(cache != NULL);

	if(objUse(regexp, LM_REGEXP_FILENAME) == RS_RET_OK) {
		if (*cache == NULL) {
			*cache = calloc(sizeof(regex_t), 1);
			regexp.regcomp(*cache, (char*) rsCStrGetSzStr(pCS1), (iType == 1 ? REG_EXTENDED : 0) | REG_NOSUB);
		}
		ret = regexp.regexec(*cache, (char*) psz, 0, NULL, 0);
		if(ret != 0)
			ABORT_FINALIZE(RS_RET_NOT_FOUND);
	} else {
		ABORT_FINALIZE(RS_RET_NOT_FOUND);
	}

finalize_it:
	RETiRet;
}