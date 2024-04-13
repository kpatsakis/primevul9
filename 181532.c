shell_gtk_embed_set_window (ShellGtkEmbed       *embed,
                            ShellEmbeddedWindow *window)
{
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);
  MetaDisplay *display = shell_global_get_display (shell_global_get ());

  if (priv->window)
    {
      if (priv->window_created_handler)
        {
          g_signal_handler_disconnect (display,
                                       priv->window_created_handler);
          priv->window_created_handler = 0;
        }

      shell_gtk_embed_remove_window_actor (embed);

      _shell_embedded_window_set_actor (priv->window, NULL);

      g_object_unref (priv->window);

      g_signal_handlers_disconnect_by_func (priv->window,
                                            (gpointer)shell_gtk_embed_on_window_destroy,
                                            embed);

      g_signal_handlers_disconnect_by_func (priv->window,
                                            (gpointer)shell_gtk_embed_on_window_mapped,
                                            embed);
    }

  priv->window = window;

  if (priv->window)
    {
      g_object_ref (priv->window);

      _shell_embedded_window_set_actor (priv->window, embed);

      g_signal_connect (priv->window, "destroy",
                        G_CALLBACK (shell_gtk_embed_on_window_destroy), embed);

      g_signal_connect (priv->window, "map",
                        G_CALLBACK (shell_gtk_embed_on_window_mapped), embed);
    }

  clutter_actor_queue_relayout (CLUTTER_ACTOR (embed));
}