static void remove_expired(struct CookieInfo *cookies)
{
  struct Cookie *co, *nx, *pv;
  curl_off_t now = (curl_off_t)time(NULL);

  co = cookies->cookies;
  pv = NULL;
  while(co) {
    nx = co->next;
    if((co->expirestr || co->maxage) && co->expires < now) {
      if(co == cookies->cookies) {
        cookies->cookies = co->next;
      }
      else {
        pv->next = co->next;
      }
      cookies->numcookies--;
      freecookie(co);
    }
    else {
      pv = co;
    }
    co = nx;
  }
}