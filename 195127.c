D_P16 (uschar *p14, uschar *in, uschar *out)
{
smbhash (out, in, p14, 0);
smbhash (out + 8, in + 8, p14 + 7, 0);
}