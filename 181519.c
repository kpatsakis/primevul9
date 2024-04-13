shell_gtk_embed_set_property (GObject         *object,
                              guint            prop_id,
                              const GValue    *value,
                              GParamSpec      *pspec)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (object);

  switch (prop_id)
    {
    case PROP_WINDOW:
      shell_gtk_embed_set_window (embed, (ShellEmbeddedWindow *)g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}