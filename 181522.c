shell_gtk_embed_get_property (GObject         *object,
                              guint            prop_id,
                              GValue          *value,
                              GParamSpec      *pspec)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (object);
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);

  switch (prop_id)
    {
    case PROP_WINDOW:
      g_value_set_object (value, priv->window);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}