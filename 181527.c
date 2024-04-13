shell_gtk_embed_on_window_mapped (GtkWidget     *object,
                                  ShellGtkEmbed *embed)
{
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);
  MetaDisplay *display = shell_global_get_display (shell_global_get ());

  if (priv->window_created_handler == 0 && priv->window_actor == NULL)
    /* Listen for new windows so we can detect when Mutter has
       created a MutterWindow for this window */
    priv->window_created_handler =
      g_signal_connect (display,
                        "window-created",
                        G_CALLBACK (shell_gtk_embed_window_created_cb),
                        embed);
}