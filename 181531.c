shell_gtk_embed_remove_window_actor (ShellGtkEmbed *embed)
{
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);

  if (priv->window_actor)
    {
      g_signal_handler_disconnect (priv->window_actor,
                                   priv->window_actor_destroyed_handler);
      priv->window_actor_destroyed_handler = 0;

      g_object_unref (priv->window_actor);
      priv->window_actor = NULL;
    }

  clutter_clone_set_source (CLUTTER_CLONE (embed), NULL);
}