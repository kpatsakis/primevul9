void rsCStrRegexDestruct(void *rc)
{
	regex_t **cache = rc;
	
	assert(cache != NULL);
	assert(*cache != NULL);

	if(objUse(regexp, LM_REGEXP_FILENAME) == RS_RET_OK) {
		regexp.regfree(*cache);
		free(*cache);
		*cache = NULL;
	}
}