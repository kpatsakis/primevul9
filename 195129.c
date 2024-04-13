E_md4hash (uschar * passwd, uschar * p16)
{
int len;
int16x wpwd[129];

/* Password cannot be longer than 128 characters */
len = strlen (CS  passwd);
if (len > 128)
  len = 128;
/* Password must be converted to NT unicode */
_my_mbstowcs (wpwd, passwd, len);
wpwd[len] = 0;               /* Ensure string is null terminated */
/* Calculate length in bytes */
len = _my_wcslen (wpwd) * sizeof (int16x);

mdfour (p16, US wpwd, len);
}