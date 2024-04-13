MODULE_ENTRY (fill_vtable) (GdkPixbufModule *module)
{
        module->load = gdk_pixbuf__tiff_image_load;
        module->begin_load = gdk_pixbuf__tiff_image_begin_load;
        module->stop_load = gdk_pixbuf__tiff_image_stop_load;
        module->load_increment = gdk_pixbuf__tiff_image_load_increment;
        module->save = gdk_pixbuf__tiff_image_save;
        module->save_to_callback = gdk_pixbuf__tiff_image_save_to_callback;
        module->is_save_option_supported = gdk_pixbuf__tiff_is_save_option_supported;
}