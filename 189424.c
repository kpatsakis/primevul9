bool ikev1_ship_chain(chunk_t *chain, int n, pb_stream *outs,
		      uint8_t type)
{
	for (int i = 0; i < n; i++) {
		if (!ikev1_ship_CERT(type, chain[i], outs))
			return false;
	}

	return true;
}