static int network_config_set_string (const oconfig_item_t *ci, /* {{{ */
    char **ret_string)
{
  char *tmp;
  if ((ci->values_num != 1)
      || (ci->values[0].type != OCONFIG_TYPE_STRING))
  {
    WARNING ("network plugin: The `%s' config option needs exactly "
        "one string argument.", ci->key);
    return (-1);
  }

  tmp = strdup (ci->values[0].value.string);
  if (tmp == NULL)
    return (-1);

  sfree (*ret_string);
  *ret_string = tmp;

  return (0);
} /* }}} int network_config_set_string */