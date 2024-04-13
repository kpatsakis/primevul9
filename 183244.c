context_set_sm_rid2pasid(struct context_entry *context, unsigned long pasid)
{
	context->hi |= pasid & ((1 << 20) - 1);
	context->hi |= (1 << 20);
}