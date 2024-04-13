static CURLcode parse_url_login(struct Curl_easy *data,
                                struct connectdata *conn,
                                char **user, char **passwd, char **options)
{
  CURLcode result = CURLE_OK;
  char *userp = NULL;
  char *passwdp = NULL;
  char *optionsp = NULL;

  /* At this point, we're hoping all the other special cases have
   * been taken care of, so conn->host.name is at most
   *    [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */

  char *ptr = strchr(conn->host.name, '@');
  char *login = conn->host.name;

  DEBUGASSERT(!**user);
  DEBUGASSERT(!**passwd);
  DEBUGASSERT(!**options);
  DEBUGASSERT(conn->handler);

  if(!ptr)
    goto out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.my.site:8021/README */
  conn->host.name = ++ptr;

  /* So the hostname is sane.  Only bother interpreting the
   * results if we could care.  It could still be wasted
   * work because it might be overtaken by the programmatically
   * set user/passwd, but doing that first adds more cases here :-(
   */

  if(data->set.use_netrc == CURL_NETRC_REQUIRED)
    goto out;

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. */
  result =
    Curl_parse_login_details(login, ptr - login - 1,
                             &userp, &passwdp,
                             (conn->handler->flags & PROTOPT_URLOPTIONS)?
                             &optionsp:NULL);
  if(result)
    goto out;

  if(userp) {
    char *newname;

    /* We have a user in the URL */
    conn->bits.userpwd_in_url = TRUE;
    conn->bits.user_passwd = TRUE; /* enable user+password */

    /* Decode the user */
    result = Curl_urldecode(data, userp, 0, &newname, NULL, FALSE);
    if(result) {
      goto out;
    }

    free(*user);
    *user = newname;
  }

  if(passwdp) {
    /* We have a password in the URL so decode it */
    char *newpasswd;
    result = Curl_urldecode(data, passwdp, 0, &newpasswd, NULL, FALSE);
    if(result) {
      goto out;
    }

    free(*passwd);
    *passwd = newpasswd;
  }

  if(optionsp) {
    /* We have an options list in the URL so decode it */
    char *newoptions;
    result = Curl_urldecode(data, optionsp, 0, &newoptions, NULL, FALSE);
    if(result) {
      goto out;
    }

    free(*options);
    *options = newoptions;
  }


  out:

  free(userp);
  free(passwdp);
  free(optionsp);

  return result;
}