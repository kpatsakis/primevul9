termrequest_any_pending()
{
    int	    i;
    time_t  now = time(NULL);

    for (i = 0; all_termrequests[i] != NULL; ++i)
    {
	if (all_termrequests[i]->tr_progress == STATUS_SENT)
	{
	    if (all_termrequests[i]->tr_start > 0 && now > 0
				    && all_termrequests[i]->tr_start + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_progress = STATUS_FAIL;
	    else
		return TRUE;
	}
    }
    return FALSE;
}