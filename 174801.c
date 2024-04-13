setup_schedule (const guchar *key_56, DES_KS ks)
{
	guchar key[8];
	int i, c, bit;

	key[0] = (key_56[0])                                 ;
	key[1] = (key_56[1] >> 1) | ((key_56[0] << 7) & 0xFF);
	key[2] = (key_56[2] >> 2) | ((key_56[1] << 6) & 0xFF);
	key[3] = (key_56[3] >> 3) | ((key_56[2] << 5) & 0xFF);
	key[4] = (key_56[4] >> 4) | ((key_56[3] << 4) & 0xFF);
	key[5] = (key_56[5] >> 5) | ((key_56[4] << 3) & 0xFF);
	key[6] = (key_56[6] >> 6) | ((key_56[5] << 2) & 0xFF);
	key[7] =                    ((key_56[6] << 1) & 0xFF);

	/* Fix parity */
	for (i = 0; i < 8; i++) {
		for (c = bit = 0; bit < 8; bit++)
			if (key[i] & (1 << bit))
				c++;
		if (!(c & 1))
			key[i] ^= 0x01;
	}

        deskey (ks, key, 0);
}