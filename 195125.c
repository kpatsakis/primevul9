dumpSmbNtlmAuthRequest (FILE * fp, SPAAuthRequest * request)
{
fprintf (fp, "NTLM Request:\n");
fprintf (fp, "      Ident = %s\n", request->ident);
fprintf (fp, "      mType = %d\n", IVAL (&request->msgType, 0));
fprintf (fp, "      Flags = %08x\n", IVAL (&request->flags, 0));
fprintf (fp, "       User = %s\n", GetString (request, user));
fprintf (fp, "     Domain = %s\n", GetString (request, domain));
}