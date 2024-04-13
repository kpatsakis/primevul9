static inline int dccp_need_reset(int state)
{
	return state != DCCP_CLOSED && state != DCCP_LISTEN &&
	       state != DCCP_REQUESTING;
}