dumpSmbNtlmAuthResponse (FILE * fp, SPAAuthResponse * response)
{
fprintf (fp, "NTLM Response:\n");
fprintf (fp, "      Ident = %s\n", response->ident);
fprintf (fp, "      mType = %d\n", IVAL (&response->msgType, 0));
fprintf (fp, "     LmResp = ");
DumpBuffer (fp, response, lmResponse);
fprintf (fp, "     NTResp = ");
DumpBuffer (fp, response, ntResponse);
fprintf (fp, "     Domain = %s\n", GetUnicodeString (response, uDomain));
fprintf (fp, "       User = %s\n", GetUnicodeString (response, uUser));
fprintf (fp, "        Wks = %s\n", GetUnicodeString (response, uWks));
fprintf (fp, "       sKey = ");
DumpBuffer (fp, response, sessionKey);
fprintf (fp, "      Flags = %08x\n", IVAL (&response->flags, 0));
}