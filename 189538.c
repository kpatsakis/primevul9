unref_object_in_thread (gpointer ptr)
{
	GObject *object = ptr;

	g_return_val_if_fail (object != NULL, NULL);

	g_object_unref (object);

	return NULL;
}