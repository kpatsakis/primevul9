static void resume_callback(void)
{
	gboolean suspend = FALSE;

	DBG("Resuming ...");

	queue_foreach(devices, set_suspend, GINT_TO_POINTER(suspend));
}