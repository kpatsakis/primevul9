shell_gtk_embed_on_window_destroy (GtkWidget     *object,
                                   ShellGtkEmbed *embed)
{
  shell_gtk_embed_set_window (embed, NULL);
}