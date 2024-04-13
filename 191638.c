static int nb_add_value_list (lcc_network_buffer_t *nb, /* {{{ */
    const lcc_value_list_t *vl)
{
  char *buffer = nb->ptr;
  size_t buffer_size = nb->free;

  const lcc_identifier_t *ident_src;
  lcc_identifier_t *ident_dst;

  ident_src = &vl->identifier;
  ident_dst = &nb->state.identifier;

  if (strcmp (ident_dst->host, ident_src->host) != 0)
  {
    if (nb_add_string (&buffer, &buffer_size, TYPE_HOST,
          ident_src->host, strlen (ident_src->host)) != 0)
      return (-1);
    SSTRNCPY (ident_dst->host, ident_src->host, sizeof (ident_dst->host));
  }

  if (strcmp (ident_dst->plugin, ident_src->plugin) != 0)
  {
    if (nb_add_string (&buffer, &buffer_size, TYPE_PLUGIN,
          ident_src->plugin, strlen (ident_src->plugin)) != 0)
      return (-1);
    SSTRNCPY (ident_dst->plugin, ident_src->plugin,
        sizeof (ident_dst->plugin));
  }

  if (strcmp (ident_dst->plugin_instance,
        ident_src->plugin_instance) != 0)
  {
    if (nb_add_string (&buffer, &buffer_size, TYPE_PLUGIN_INSTANCE,
          ident_src->plugin_instance,
          strlen (ident_src->plugin_instance)) != 0)
      return (-1);
    SSTRNCPY (ident_dst->plugin_instance, ident_src->plugin_instance,
        sizeof (ident_dst->plugin_instance));
  }

  if (strcmp (ident_dst->type, ident_src->type) != 0)
  {
    if (nb_add_string (&buffer, &buffer_size, TYPE_TYPE,
          ident_src->type, strlen (ident_src->type)) != 0)
      return (-1);
    SSTRNCPY (ident_dst->type, ident_src->type, sizeof (ident_dst->type));
  }

  if (strcmp (ident_dst->type_instance,
        ident_src->type_instance) != 0)
  {
    if (nb_add_string (&buffer, &buffer_size, TYPE_TYPE_INSTANCE,
          ident_src->type_instance,
          strlen (ident_src->type_instance)) != 0)
      return (-1);
    SSTRNCPY (ident_dst->type_instance, ident_src->type_instance,
        sizeof (ident_dst->type_instance));
  }

  if (nb->state.time != vl->time)
  {
    if (nb_add_time (&buffer, &buffer_size, TYPE_TIME_HR, vl->time))
      return (-1);
    nb->state.time = vl->time;
  }

  if (nb->state.interval != vl->interval)
  {
    if (nb_add_time (&buffer, &buffer_size, TYPE_INTERVAL_HR, vl->interval))
      return (-1);
    nb->state.interval = vl->interval;
  }

  if (nb_add_values (&buffer, &buffer_size, vl) != 0)
    return (-1);

  nb->ptr = buffer;
  nb->free = buffer_size;
  return (0);
} /* }}} int nb_add_value_list */