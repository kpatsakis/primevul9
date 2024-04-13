auth_call_pam(const uschar *s, uschar **errptr)
{
pam_handle_t *pamh = NULL;
struct pam_conv pamc;
int pam_error;
int sep = 0;
uschar *user;

/* Set up the input data structure: the address of the conversation function,
and a pointer to application data, which we don't use because I couldn't get it
to work under Solaris 2.6 - it always arrived in pam_converse() as NULL. */

pamc.conv = pam_converse;
pamc.appdata_ptr = NULL;

/* Initialize the static data - the current input data, the error flag, and the
flag for data end. */

pam_args = s;
pam_conv_had_error = FALSE;
pam_arg_ended = FALSE;

/* The first string in the list is the user. If this is an empty string, we
fail. PAM doesn't support authentication with an empty user (it prompts for it,
causing a potential mis-interpretation). */

user = string_nextinlist(&pam_args, &sep, NULL, 0);
if (user == NULL || user[0] == 0) return FAIL;

/* Start off PAM interaction */

DEBUG(D_auth)
  debug_printf("Running PAM authentication for user \"%s\"\n", user);

pam_error = pam_start ("exim", CS user, &pamc, &pamh);

/* Do the authentication - the pam_authenticate() will call pam_converse() to
get the data it wants. After successful authentication we call pam_acct_mgmt()
to apply any other restrictions (e.g. only some times of day). */

if (pam_error == PAM_SUCCESS)
  {
  pam_error = pam_authenticate (pamh, PAM_SILENT);
  if (pam_error == PAM_SUCCESS && !pam_conv_had_error)
    pam_error = pam_acct_mgmt (pamh, PAM_SILENT);
  }

/* Finish the PAM interaction - this causes it to clean up store etc. Unclear
what should be passed as the second argument. */

pam_end(pamh, PAM_SUCCESS);

/* Sort out the return code. If not success, set the error message. */

if (pam_error == PAM_SUCCESS)
  {
  DEBUG(D_auth) debug_printf("PAM success\n");
  return OK;
  }

*errptr = US pam_strerror(pamh, pam_error);
DEBUG(D_auth) debug_printf("PAM error: %s\n", *errptr);

if (pam_error == PAM_USER_UNKNOWN ||
    pam_error == PAM_AUTH_ERR ||
    pam_error == PAM_ACCT_EXPIRED)
  return FAIL;

return ERROR;
}