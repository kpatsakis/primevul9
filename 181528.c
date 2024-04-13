shell_gtk_embed_allocate (ClutterActor          *actor,
                          const ClutterActorBox *box,
                          ClutterAllocationFlags flags)
{
  ShellGtkEmbed *embed = SHELL_GTK_EMBED (actor);
  ShellGtkEmbedPrivate *priv = shell_gtk_embed_get_instance_private (embed);
  float wx = 0.0, wy = 0.0, x, y, ax, ay;

  CLUTTER_ACTOR_CLASS (shell_gtk_embed_parent_class)->
    allocate (actor, box, flags);

  /* Find the actor's new coordinates in terms of the stage (which is
   * priv->window's parent window.
   */
  while (actor)
    {
      clutter_actor_get_position (actor, &x, &y);
      clutter_actor_get_anchor_point (actor, &ax, &ay);

      wx += x - ax;
      wy += y - ay;

      actor = clutter_actor_get_parent (actor);
    }

  _shell_embedded_window_allocate (priv->window,
                                   (int)(0.5 + wx), (int)(0.5 + wy),
                                   box->x2 - box->x1,
                                   box->y2 - box->y1);
}