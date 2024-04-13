E_P24 (uschar *p21, uschar *c8, uschar *p24)
{
smbhash (p24, c8, p21, 1);
smbhash (p24 + 8, c8, p21 + 7, 1);
smbhash (p24 + 16, c8, p21 + 14, 1);
}