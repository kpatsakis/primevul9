calc_response (const guchar *key, const guchar *plaintext, guchar *results)
{
        DES_KS ks;

	memcpy (results, plaintext, 8);
	memcpy (results + 8, plaintext, 8);
	memcpy (results + 16, plaintext, 8);

        setup_schedule (key, ks);
	des (ks, results);

        setup_schedule (key + 7, ks);
	des (ks, results + 8);

        setup_schedule (key + 14, ks);
        des (ks, results + 16);
}