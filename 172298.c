static void set_suspend(gpointer data, gpointer user_data)
{
	struct hog_device *dev = data;
	gboolean suspend = GPOINTER_TO_INT(user_data);

	bt_hog_set_control_point(dev->hog, suspend);
}