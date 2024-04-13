void Curl_cookie_clearall(struct CookieInfo *cookies)
{
  if(cookies) {
    Curl_cookie_freelist(cookies->cookies, TRUE);
    cookies->cookies = NULL;
    cookies->numcookies = 0;
  }
}