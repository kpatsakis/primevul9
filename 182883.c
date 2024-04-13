mctxfree(void **target) {
	isc_mem_t *mctx = *(isc_mem_t **) target;
	isc_mem_detach(&mctx);
	*target = NULL;
}