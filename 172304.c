static void suspend_callback(void)
{
	gboolean suspend = TRUE;

	DBG("Suspending ...");

	queue_foreach(devices, set_suspend, GINT_TO_POINTER(suspend));
}