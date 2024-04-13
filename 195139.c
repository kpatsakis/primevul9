SMBOWFencrypt (uschar passwd[16], uschar * c8, uschar p24[24])
{
uschar p21[21];

memset (p21, '\0', 21);

memcpy (p21, passwd, 16);
E_P24 (p21, c8, p24);
}