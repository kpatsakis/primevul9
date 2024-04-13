int mutt_can_decode (BODY *a)
{
  if (mutt_is_autoview (a))
    return 1;
  else if (a->type == TYPETEXT)
    return (1);
  else if (a->type == TYPEMESSAGE)
    return (1);
  else if (a->type == TYPEMULTIPART)
  {
    BODY *p;

    if (WithCrypto)
    {
      if (ascii_strcasecmp (a->subtype, "signed") == 0 ||
	  ascii_strcasecmp (a->subtype, "encrypted") == 0)
        return (1);
    }

    for (p = a->parts; p; p = p->next)
    {
      if (mutt_can_decode (p))
        return (1);
    }

  }
  else if (WithCrypto && a->type == TYPEAPPLICATION)
  {
    if ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp(a))
      return (1);
    if ((WithCrypto & APPLICATION_SMIME) && mutt_is_application_smime(a))
      return (1);
  }

  return (0);
}