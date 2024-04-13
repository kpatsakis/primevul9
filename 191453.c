MagickExport MagickBooleanType DecipherImage(Image *image,
  const char *passphrase,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  StringInfo
    *passkey;

  if (passphrase == (const char *) NULL)
    return(MagickTrue);
  passkey=StringToStringInfo(passphrase);
  if (passkey == (StringInfo *) NULL)
    return(MagickFalse);
  status=PasskeyDecipherImage(image,passkey,exception);
  passkey=DestroyStringInfo(passkey);
  return(status);
}