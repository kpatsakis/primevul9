rdpsnd_auto_select(void)
{
	static RD_BOOL failed = False;

	if (!failed)
	{
		current_driver = drivers;
		while (current_driver != NULL)
		{
			DEBUG(("trying %s...\n", current_driver->name));
			if (current_driver->wave_out_open())
			{
				DEBUG(("selected %s\n", current_driver->name));
				current_driver->wave_out_close();
				return True;
			}
			current_driver = current_driver->next;
		}

		warning("no working audio-driver found\n");
		failed = True;
		current_driver = NULL;
	}

	return False;
}