g_file_supports_thread_contexts (GFile *file)
{
 GFileIface *iface;

 g_return_val_if_fail (G_IS_FILE (file), FALSE);

 iface = G_FILE_GET_IFACE (file);
 return iface->supports_thread_contexts;
}