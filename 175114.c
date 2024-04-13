void Curl_cookie_freelist(struct Cookie *co, bool cookiestoo)
{
  struct Cookie *next;
  while(co) {
    next = co->next;
    if(cookiestoo)
      freecookie(co);
    else
      free(co); /* we only free the struct since the "members" are all just
                   pointed out in the main cookie list! */
    co = next;
  }
}