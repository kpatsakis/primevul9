spa_smb_nt_encrypt (uschar * passwd, uschar * c8, uschar * p24)
{
uschar p21[21];

memset (p21, '\0', 21);

E_md4hash (passwd, p21);
SMBOWFencrypt (p21, c8, p24);

#ifdef DEBUG_PASSWORD
DEBUG_X (100, ("spa_smb_nt_encrypt: nt#, challenge, response\n"));
dump_data (100, CS  p21, 16);
dump_data (100, CS  c8, 8);
dump_data (100, CS  p24, 24);
#endif
}