NTLMSSPOWFencrypt (uschar passwd[8], uschar * ntlmchalresp, uschar p24[24])
{
uschar p21[21];

memset (p21, '\0', 21);
memcpy (p21, passwd, 8);
memset (p21 + 8, 0xbd, 8);

E_P24 (p21, ntlmchalresp, p24);
#ifdef DEBUG_PASSWORD
DEBUG_X (100, ("NTLMSSPOWFencrypt: p21, c8, p24\n"));
dump_data (100, CS  p21, 21);
dump_data (100, CS  ntlmchalresp, 8);
dump_data (100, CS  p24, 24);
#endif
}