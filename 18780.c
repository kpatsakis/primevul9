req_more_codes_from_term(void)
{
    char	buf[23];  // extra size to shut up LGTM
    int		old_idx = xt_index_out;

    // Don't do anything when going to exit.
    if (exiting)
	return;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    while (xt_index_out < xt_index_in + 10 && key_names[xt_index_out] != NULL)
    {
	char *key_name = key_names[xt_index_out];

	MAY_WANT_TO_LOG_THIS;
	LOG_TR(("Requesting XT %d: %s", xt_index_out, key_name));
	sprintf(buf, "\033P+q%02x%02x\033\\", key_name[0], key_name[1]);
	out_str_nf((char_u *)buf);
	++xt_index_out;
    }

    // Send the codes out right away.
    if (xt_index_out != old_idx)
	out_flush();
}