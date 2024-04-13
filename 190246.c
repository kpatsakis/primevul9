int SetHTTPrequest(struct OperationConfig *config, HttpReq req, HttpReq *store)
{
  /* this mirrors the HttpReq enum in tool_sdecls.h */
  const char *reqname[]= {
    "", /* unspec */
    "GET (-G, --get)",
    "HEAD (-I, --head)",
    "multipart formpost (-F, --form)",
    "POST (-d, --data)"
  };

  if((*store == HTTPREQ_UNSPEC) ||
     (*store == req)) {
    *store = req;
    return 0;
  }
  warnf(config->global, "You can only select one HTTP request method! "
        "You asked for both %s and %s.\n",
        reqname[req], reqname[*store]);

  return 1;
}