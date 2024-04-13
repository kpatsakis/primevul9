getinfo_helper_policies(control_connection_t *conn,
                        const char *question, char **answer)
{
  (void) conn;
  if (!strcmp(question, "exit-policy/default")) {
    *answer = tor_strdup(DEFAULT_EXIT_POLICY);
  }
  return 0;
}