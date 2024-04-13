void Curl_cookie_clearsess(struct CookieInfo *cookies)
{
  struct Cookie *first, *curr, *next, *prev = NULL;

  if(!cookies || !cookies->cookies)
    return;

  first = curr = prev = cookies->cookies;

  for(; curr; curr = next) {
    next = curr->next;
    if(!curr->expires) {
      if(first == curr)
        first = next;

      if(prev == curr)
        prev = next;
      else
        prev->next = next;

      freecookie(curr);
      cookies->numcookies--;
    }
    else
      prev = curr;
  }

  cookies->cookies = first;
}