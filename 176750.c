static enum loglevel string_to_loglevel(const char *s) {
  if (strncmp(s, "ERR:", 4) == 0)
    return ERROR_LEVEL;
  else if (strncmp(s, "WRN:", 4) == 0)
    return WARNING_LEVEL;
  else if (strncmp(s, "INF:", 4) == 0)
    return INFORMATION_LEVEL;
  else if (strncmp(s, "DBG:", 4) == 0)
    return INFORMATION_LEVEL;
  else
    return SYSTEM_LEVEL;
}