shell_gtk_embed_new (ShellEmbeddedWindow *window)
{
  g_return_val_if_fail (SHELL_IS_EMBEDDED_WINDOW (window), NULL);

  return g_object_new (SHELL_TYPE_GTK_EMBED,
                       "window", window,
                       NULL);
}