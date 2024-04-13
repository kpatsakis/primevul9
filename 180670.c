bool context_present(struct context_entry *context)
{
	return context_pasid_enabled(context) ?
	     __context_present(context) :
	     __context_present(context) && !context_copied(context);
}