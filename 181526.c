shell_gtk_embed_class_init (ShellGtkEmbedClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  object_class->get_property = shell_gtk_embed_get_property;
  object_class->set_property = shell_gtk_embed_set_property;
  object_class->dispose      = shell_gtk_embed_dispose;

  actor_class->get_preferred_width = shell_gtk_embed_get_preferred_width;
  actor_class->get_preferred_height = shell_gtk_embed_get_preferred_height;
  actor_class->allocate = shell_gtk_embed_allocate;
  actor_class->map = shell_gtk_embed_map;
  actor_class->unmap = shell_gtk_embed_unmap;

  g_object_class_install_property (object_class,
                                   PROP_WINDOW,
                                   g_param_spec_object ("window",
                                                        "Window",
                                                        "ShellEmbeddedWindow to embed",
                                                        SHELL_TYPE_EMBEDDED_WINDOW,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}