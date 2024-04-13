spa_smb_encrypt (uschar * passwd, uschar * c8, uschar * p24)
{
uschar p14[15], p21[21];

memset (p21, '\0', 21);
memset (p14, '\0', 14);
StrnCpy (CS  p14, CS  passwd, 14);

strupper (CS  p14);
E_P16 (p14, p21);

SMBOWFencrypt (p21, c8, p24);

#ifdef DEBUG_PASSWORD
DEBUG_X (100, ("spa_smb_encrypt: lm#, challenge, response\n"));
dump_data (100, CS  p21, 16);
dump_data (100, CS  c8, 8);
dump_data (100, CS  p24, 24);
#endif
}