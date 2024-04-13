shell_gtk_embed_get_preferred_width (ClutterActor *actor,
                                     float         for_height,
                                     float        *min_width_p,
                                     float        *natural_width_p)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (actor);
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);

  if (priv->window
      && gtk_widget_get_visible (GTK_WIDGET (priv->window)))
    {
      GtkRequisition min_req, natural_req;
      gtk_widget_get_preferred_size (GTK_WIDGET (priv->window), &min_req, &natural_req);

      *min_width_p = min_req.width;
      *natural_width_p = natural_req.width;
    }
  else
    *min_width_p = *natural_width_p = 0;
}