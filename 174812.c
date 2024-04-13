md4sum (const unsigned char *in, int nbytes, unsigned char digest[16])
{
	unsigned char *M;
	guint32 A, B, C, D, AA, BB, CC, DD, X[16];
	int pbytes, nbits = nbytes * 8, i, j;

	/* There is *always* padding of at least one bit. */
	pbytes = ((119 - (nbytes % 64)) % 64) + 1;
	M = alloca (nbytes + pbytes + 8);
	memcpy (M, in, nbytes);
	memset (M + nbytes, 0, pbytes + 8);
	M[nbytes] = 0x80;
	M[nbytes + pbytes] = nbits & 0xFF;
	M[nbytes + pbytes + 1] = (nbits >> 8) & 0xFF;
	M[nbytes + pbytes + 2] = (nbits >> 16) & 0xFF;
	M[nbytes + pbytes + 3] = (nbits >> 24) & 0xFF;

	A = 0x67452301;
	B = 0xEFCDAB89;
	C = 0x98BADCFE;
	D = 0x10325476;

	for (i = 0; i < nbytes + pbytes + 8; i += 64) {
		for (j = 0; j < 16; j++) {
			X[j] =  (M[i + j*4]) |
				(M[i + j*4 + 1] << 8) |
				(M[i + j*4 + 2] << 16) |
				(M[i + j*4 + 3] << 24);
		}

		AA = A;
		BB = B;
		CC = C;
		DD = D;

		A = ROT (A + F(B, C, D) + X[0], 3);
		D = ROT (D + F(A, B, C) + X[1], 7);
		C = ROT (C + F(D, A, B) + X[2], 11);
		B = ROT (B + F(C, D, A) + X[3], 19);
		A = ROT (A + F(B, C, D) + X[4], 3);
		D = ROT (D + F(A, B, C) + X[5], 7);
		C = ROT (C + F(D, A, B) + X[6], 11);
		B = ROT (B + F(C, D, A) + X[7], 19);
		A = ROT (A + F(B, C, D) + X[8], 3);
		D = ROT (D + F(A, B, C) + X[9], 7);
		C = ROT (C + F(D, A, B) + X[10], 11);
		B = ROT (B + F(C, D, A) + X[11], 19);
		A = ROT (A + F(B, C, D) + X[12], 3);
		D = ROT (D + F(A, B, C) + X[13], 7);
		C = ROT (C + F(D, A, B) + X[14], 11);
		B = ROT (B + F(C, D, A) + X[15], 19);

		A = ROT (A + G(B, C, D) + X[0] + 0x5A827999, 3);
		D = ROT (D + G(A, B, C) + X[4] + 0x5A827999, 5);
		C = ROT (C + G(D, A, B) + X[8] + 0x5A827999, 9);
		B = ROT (B + G(C, D, A) + X[12] + 0x5A827999, 13);
		A = ROT (A + G(B, C, D) + X[1] + 0x5A827999, 3);
		D = ROT (D + G(A, B, C) + X[5] + 0x5A827999, 5);
		C = ROT (C + G(D, A, B) + X[9] + 0x5A827999, 9);
		B = ROT (B + G(C, D, A) + X[13] + 0x5A827999, 13);
		A = ROT (A + G(B, C, D) + X[2] + 0x5A827999, 3);
		D = ROT (D + G(A, B, C) + X[6] + 0x5A827999, 5);
		C = ROT (C + G(D, A, B) + X[10] + 0x5A827999, 9);
		B = ROT (B + G(C, D, A) + X[14] + 0x5A827999, 13);
		A = ROT (A + G(B, C, D) + X[3] + 0x5A827999, 3);
		D = ROT (D + G(A, B, C) + X[7] + 0x5A827999, 5);
		C = ROT (C + G(D, A, B) + X[11] + 0x5A827999, 9);
		B = ROT (B + G(C, D, A) + X[15] + 0x5A827999, 13);

		A = ROT (A + H(B, C, D) + X[0] + 0x6ED9EBA1, 3);
		D = ROT (D + H(A, B, C) + X[8] + 0x6ED9EBA1, 9);
		C = ROT (C + H(D, A, B) + X[4] + 0x6ED9EBA1, 11);
		B = ROT (B + H(C, D, A) + X[12] + 0x6ED9EBA1, 15);
		A = ROT (A + H(B, C, D) + X[2] + 0x6ED9EBA1, 3);
		D = ROT (D + H(A, B, C) + X[10] + 0x6ED9EBA1, 9);
		C = ROT (C + H(D, A, B) + X[6] + 0x6ED9EBA1, 11);
		B = ROT (B + H(C, D, A) + X[14] + 0x6ED9EBA1, 15);
		A = ROT (A + H(B, C, D) + X[1] + 0x6ED9EBA1, 3);
		D = ROT (D + H(A, B, C) + X[9] + 0x6ED9EBA1, 9);
		C = ROT (C + H(D, A, B) + X[5] + 0x6ED9EBA1, 11);
		B = ROT (B + H(C, D, A) + X[13] + 0x6ED9EBA1, 15);
		A = ROT (A + H(B, C, D) + X[3] + 0x6ED9EBA1, 3);
		D = ROT (D + H(A, B, C) + X[11] + 0x6ED9EBA1, 9);
		C = ROT (C + H(D, A, B) + X[7] + 0x6ED9EBA1, 11);
		B = ROT (B + H(C, D, A) + X[15] + 0x6ED9EBA1, 15);

		A += AA;
		B += BB;
		C += CC;
		D += DD;
	}

	digest[0]  =  A        & 0xFF;
	digest[1]  = (A >>  8) & 0xFF;
	digest[2]  = (A >> 16) & 0xFF;
	digest[3]  = (A >> 24) & 0xFF;
	digest[4]  =  B        & 0xFF;
	digest[5]  = (B >>  8) & 0xFF;
	digest[6]  = (B >> 16) & 0xFF;
	digest[7]  = (B >> 24) & 0xFF;
	digest[8]  =  C        & 0xFF;
	digest[9]  = (C >>  8) & 0xFF;
	digest[10] = (C >> 16) & 0xFF;
	digest[11] = (C >> 24) & 0xFF;
	digest[12] =  D        & 0xFF;
	digest[13] = (D >>  8) & 0xFF;
	digest[14] = (D >> 16) & 0xFF;
	digest[15] = (D >> 24) & 0xFF;
}