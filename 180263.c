static int network_config_set_security_level(oconfig_item_t *ci, /* {{{ */
                                             int *retval) {
  char *str;
  if ((ci->values_num != 1) || (ci->values[0].type != OCONFIG_TYPE_STRING)) {
    WARNING("network plugin: The `SecurityLevel' config option needs exactly "
            "one string argument.");
    return (-1);
  }

  str = ci->values[0].value.string;
  if (strcasecmp("Encrypt", str) == 0)
    *retval = SECURITY_LEVEL_ENCRYPT;
  else if (strcasecmp("Sign", str) == 0)
    *retval = SECURITY_LEVEL_SIGN;
  else if (strcasecmp("None", str) == 0)
    *retval = SECURITY_LEVEL_NONE;
  else {
    WARNING("network plugin: Unknown security level: %s.", str);
    return (-1);
  }

  return (0);
} /* }}} int network_config_set_security_level */