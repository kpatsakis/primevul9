find_method_icall (const IcallTypeDesc *imap, const char *name)
{
	const char **nameslot = bsearch (name, icall_names + imap->first_icall, icall_desc_num_icalls (imap), sizeof (icall_names [0]), compare_method_imap);
	if (!nameslot)
		return NULL;
	return (gpointer)icall_functions [(nameslot - icall_names)];
}