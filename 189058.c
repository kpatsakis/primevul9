static const char * _pam_get_item_byname(pam_handle_t *pamh, const char *name)
{
  /*
   * This function just allows me to use names as given in the config
   * file and translate them into the appropriate PAM_ITEM macro
   */

  int item;
  const void *itemval;

  D(("Called."));
  if (strcmp(name, "PAM_USER") == 0) {
    item = PAM_USER;
  } else if (strcmp(name, "PAM_USER_PROMPT") == 0) {
    item = PAM_USER_PROMPT;
  } else if (strcmp(name, "PAM_TTY") == 0) {
    item = PAM_TTY;
  } else if (strcmp(name, "PAM_RUSER") == 0) {
    item = PAM_RUSER;
  } else if (strcmp(name, "PAM_RHOST") == 0) {
    item = PAM_RHOST;
  } else {
    D(("Unknown PAM_ITEM: <%s>", name));
    pam_syslog (pamh, LOG_ERR, "Unknown PAM_ITEM: <%s>", name);
    return NULL;
  }

  if (pam_get_item(pamh, item, &itemval) != PAM_SUCCESS) {
    D(("pam_get_item failed"));
    return NULL;     /* let pam_get_item() log the error */
  }
  D(("Exit."));
  return itemval;
}