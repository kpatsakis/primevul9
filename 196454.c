static bool pickoneauth(struct auth *pick)
{
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  if(avail & CURLAUTH_NEGOTIATE)
    pick->picked = CURLAUTH_NEGOTIATE;
  else if(avail & CURLAUTH_DIGEST)
    pick->picked = CURLAUTH_DIGEST;
  else if(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
  else if(avail & CURLAUTH_NTLM_WB)
    pick->picked = CURLAUTH_NTLM_WB;
  else if(avail & CURLAUTH_BASIC)
    pick->picked = CURLAUTH_BASIC;
  else {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  return picked;
}