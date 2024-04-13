static int network_config_set_boolean (const oconfig_item_t *ci, /* {{{ */
    int *retval)
{
  if ((ci->values_num != 1)
      || ((ci->values[0].type != OCONFIG_TYPE_BOOLEAN)
        && (ci->values[0].type != OCONFIG_TYPE_STRING)))
  {
    ERROR ("network plugin: The `%s' config option needs "
        "exactly one boolean argument.", ci->key);
    return (-1);
  }

  if (ci->values[0].type == OCONFIG_TYPE_BOOLEAN)
  {
    if (ci->values[0].value.boolean)
      *retval = 1;
    else
      *retval = 0;
  }
  else
  {
    char *str = ci->values[0].value.string;

    if (IS_TRUE (str))
      *retval = 1;
    else if (IS_FALSE (str))
      *retval = 0;
    else
    {
      ERROR ("network plugin: Cannot parse string value `%s' of the `%s' "
          "option as boolean value.",
          str, ci->key);
      return (-1);
    }
  }

  return (0);
} /* }}} int network_config_set_boolean */