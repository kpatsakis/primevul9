sec_send_to_channel(STREAM s, uint32 flags, uint16 channel)
{
	int datalen;

#ifdef WITH_SCARD
	scard_lock(SCARD_LOCK_SEC);
#endif

	s_pop_layer(s, sec_hdr);
	if ((!g_licence_issued && !g_licence_error_result) || (flags & SEC_ENCRYPT))
		out_uint32_le(s, flags);

	if (flags & SEC_ENCRYPT)
	{
		flags &= ~SEC_ENCRYPT;
		datalen = s->end - s->p - 8;

#if WITH_DEBUG
		DEBUG(("Sending encrypted packet:\n"));
		hexdump(s->p + 8, datalen);
#endif

		sec_sign(s->p, 8, g_sec_sign_key, g_rc4_key_len, s->p + 8, datalen);
		sec_encrypt(s->p + 8, datalen);
	}

	mcs_send_to_channel(s, channel);

#ifdef WITH_SCARD
	scard_unlock(SCARD_LOCK_SEC);
#endif
}