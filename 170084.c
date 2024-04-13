MODULE_ENTRY (fill_vtable) (GdkPixbufModule *module)
{
	module->begin_load = gdk_pixbuf__bmp_image_begin_load;
	module->stop_load = gdk_pixbuf__bmp_image_stop_load;
	module->load_increment = gdk_pixbuf__bmp_image_load_increment;
	module->save = gdk_pixbuf__bmp_image_save;
	module->save_to_callback = gdk_pixbuf__bmp_image_save_to_callback;
}