static const char *_get_ntstatus_error_string(const char *nt_status_string)
{
	int i;
	for (i=0; ntstatus_errors[i].ntstatus_string != NULL; i++) {
		if (!strcasecmp(ntstatus_errors[i].ntstatus_string,
				nt_status_string)) {
			return _(ntstatus_errors[i].error_string);
		}
	}
	return NULL;
}