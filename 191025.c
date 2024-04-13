const char *smb_fn_name(int type)
{
	const char *unknown_name = "SMBunknown";

	if (smb_messages[type].name == NULL)
		return(unknown_name);

	return(smb_messages[type].name);
}