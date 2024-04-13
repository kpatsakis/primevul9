shell_gtk_embed_unmap (ClutterActor *actor)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (actor);
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);

  _shell_embedded_window_unmap (priv->window);

  CLUTTER_ACTOR_CLASS (shell_gtk_embed_parent_class)->unmap (actor);
}