shell_gtk_embed_dispose (GObject *object)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (object);

  G_OBJECT_CLASS (shell_gtk_embed_parent_class)->dispose (object);

  shell_gtk_embed_set_window (embed, NULL);
}