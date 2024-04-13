static char *_pam_delete(register char *xx)
{
	_pam_overwrite(xx);
	_pam_drop(xx);
	return NULL;
}