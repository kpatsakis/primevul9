smb2_select_sectype(struct TCP_Server_Info *server, enum securityEnum requested)
{
	switch (requested) {
	case Kerberos:
	case RawNTLMSSP:
		return requested;
	case NTLMv2:
		return RawNTLMSSP;
	case Unspecified:
		if (server->sec_ntlmssp &&
			(global_secflags & CIFSSEC_MAY_NTLMSSP))
			return RawNTLMSSP;
		if ((server->sec_kerberos || server->sec_mskerberos) &&
			(global_secflags & CIFSSEC_MAY_KRB5))
			return Kerberos;
		/* Fallthrough */
	default:
		return Unspecified;
	}
}