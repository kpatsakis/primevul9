rdpsnd_show_help(void)
{
	struct audio_driver *pos;

	rdpsnd_register_drivers(NULL);

	pos = drivers;
	while (pos != NULL)
	{
		fprintf(stderr, "                     %s:\t%s\n", pos->name, pos->description);
		pos = pos->next;
	}
}