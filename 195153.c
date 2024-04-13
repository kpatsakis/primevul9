dumpSmbNtlmAuthChallenge (FILE * fp, SPAAuthChallenge * challenge)
{
fprintf (fp, "NTLM Challenge:\n");
fprintf (fp, "      Ident = %s\n", challenge->ident);
fprintf (fp, "      mType = %d\n", IVAL (&challenge->msgType, 0));
fprintf (fp, "     Domain = %s\n", GetUnicodeString (challenge, uDomain));
fprintf (fp, "      Flags = %08x\n", IVAL (&challenge->flags, 0));
fprintf (fp, "  Challenge = ");
dumpRaw (fp, challenge->challengeData, 8);
}