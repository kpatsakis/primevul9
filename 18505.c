can_prompt_again(secuPWData *pwdata)
{
	if (pwdata->orig_source == PW_PROMPT)
		return true;

	if (pwdata->source == PW_DEVICE)
		return true;

	return false;
}