static CURLcode parse_proxy_auth(struct Curl_easy *data,
                                 struct connectdata *conn)
{
  char proxyuser[MAX_CURL_USER_LENGTH]="";
  char proxypasswd[MAX_CURL_PASSWORD_LENGTH]="";
  CURLcode result;

  if(data->set.str[STRING_PROXYUSERNAME] != NULL) {
    strncpy(proxyuser, data->set.str[STRING_PROXYUSERNAME],
            MAX_CURL_USER_LENGTH);
    proxyuser[MAX_CURL_USER_LENGTH-1] = '\0';   /*To be on safe side*/
  }
  if(data->set.str[STRING_PROXYPASSWORD] != NULL) {
    strncpy(proxypasswd, data->set.str[STRING_PROXYPASSWORD],
            MAX_CURL_PASSWORD_LENGTH);
    proxypasswd[MAX_CURL_PASSWORD_LENGTH-1] = '\0'; /*To be on safe side*/
  }

  result = Curl_urldecode(data, proxyuser, 0, &conn->http_proxy.user, NULL,
                          FALSE);
  if(!result)
    result = Curl_urldecode(data, proxypasswd, 0, &conn->http_proxy.passwd,
                            NULL, FALSE);
  return result;
}