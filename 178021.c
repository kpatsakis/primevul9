static CURLcode findprotocol(struct Curl_easy *data,
                             struct connectdata *conn,
                             const char *protostr)
{
  const struct Curl_handler * const *pp;
  const struct Curl_handler *p;

  /* Scan protocol handler table and match against 'protostr' to set a few
     variables based on the URL. Now that the handler may be changed later
     when the protocol specific setup function is called. */
  for(pp = protocols; (p = *pp) != NULL; pp++) {
    if(strcasecompare(p->scheme, protostr)) {
      /* Protocol found in table. Check if allowed */
      if(!(data->set.allowed_protocols & p->protocol))
        /* nope, get out */
        break;

      /* it is allowed for "normal" request, now do an extra check if this is
         the result of a redirect */
      if(data->state.this_is_a_follow &&
         !(data->set.redir_protocols & p->protocol))
        /* nope, get out */
        break;

      /* Perform setup complement if some. */
      conn->handler = conn->given = p;

      /* 'port' and 'remote_port' are set in setup_connection_internals() */
      return CURLE_OK;
    }
  }


  /* The protocol was not found in the table, but we don't have to assign it
     to anything since it is already assigned to a dummy-struct in the
     create_conn() function when the connectdata struct is allocated. */
  failf(data, "Protocol \"%s\" not supported or disabled in " LIBCURL_NAME,
        protostr);

  return CURLE_UNSUPPORTED_PROTOCOL;
}