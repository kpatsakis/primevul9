MsgGetSeverity(obj_t_ptr pThis, int *piSeverity)
{
	ISOBJ_TYPE_assert(pThis, msg);
	assert(piSeverity != NULL);
	*piSeverity = ((msg_t*) pThis)->iSeverity;
	return RS_RET_OK;
}