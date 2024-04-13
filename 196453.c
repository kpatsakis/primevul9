static CURLcode http_output_basic(struct connectdata *conn, bool proxy)
{
  size_t size = 0;
  char *authorization = NULL;
  struct SessionHandle *data = conn->data;
  char **userp;
  const char *user;
  const char *pwd;
  CURLcode result;

  if(proxy) {
    userp = &conn->allocptr.proxyuserpwd;
    user = conn->proxyuser;
    pwd = conn->proxypasswd;
  }
  else {
    userp = &conn->allocptr.userpwd;
    user = conn->user;
    pwd = conn->passwd;
  }

  snprintf(data->state.buffer, sizeof(data->state.buffer), "%s:%s", user, pwd);

  result = Curl_base64_encode(data,
                              data->state.buffer, strlen(data->state.buffer),
                              &authorization, &size);
  if(result)
    return result;

  if(!authorization)
    return CURLE_REMOTE_ACCESS_DENIED;

  free(*userp);
  *userp = aprintf("%sAuthorization: Basic %s\r\n",
                   proxy?"Proxy-":"",
                   authorization);
  free(authorization);
  if(!*userp)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}