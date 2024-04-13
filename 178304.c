static void remove_listener(gpointer key, gpointer value, gpointer user_data)
{
	int index = GPOINTER_TO_INT(key);
	struct listener_data *ifdata = value;

	DBG("index %d", index);

	destroy_listener(ifdata);
}