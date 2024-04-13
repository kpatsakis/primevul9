seamless_get_token(char **s)
{
	char *comma, *head;
	head = *s;

	if (!head)
		return NULL;

	comma = strchr(head, ',');
	if (comma)
	{
		*comma = '\0';
		*s = comma + 1;
	}
	else
	{
		*s = NULL;
	}

	return head;
}