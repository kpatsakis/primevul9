bool ValidateBlowfish()
{
	std::cout << "\nBlowfish validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true, fail;

	BlowfishEncryption enc1;	// 32 to 448-bits (4 to 56-bytes)
	pass1 = enc1.StaticGetValidKeyLength(3) == 4 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(4) == 4 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(5) == 5 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(8) == 8 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(24) == 24 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(32) == 32 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(56) == 56 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(57) == 56 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(60) == 56 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(64) == 56 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(128) == 56 && pass1;

	BlowfishDecryption dec1; // 32 to 448-bits (4 to 56-bytes)
	pass2 = dec1.StaticGetValidKeyLength(3) == 4 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(4) == 4 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(5) == 5 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(8) == 8 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(24) == 24 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(32) == 32 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(56) == 56 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(57) == 56 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(60) == 56 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(64) == 56 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(128) == 56 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	HexEncoder output(new FileSink(std::cout));
	const char *key[]={"abcdefghijklmnopqrstuvwxyz", "Who is John Galt?"};
	byte *plain[]={(byte *)"BLOWFISH", (byte *)"\xfe\xdc\xba\x98\x76\x54\x32\x10"};
	byte *cipher[]={(byte *)"\x32\x4e\xd0\xfe\xf4\x13\xa2\x03", (byte *)"\xcc\x91\x73\x2b\x80\x22\xf6\x84"};
	byte out[8], outplain[8];

	for (int i=0; i<2; i++)
	{
		ECB_Mode<Blowfish>::Encryption enc2((byte *)key[i], strlen(key[i]));
		enc2.ProcessData(out, plain[i], 8);
		fail = memcmp(out, cipher[i], 8) != 0;

		ECB_Mode<Blowfish>::Decryption dec2((byte *)key[i], strlen(key[i]));
		dec2.ProcessData(outplain, cipher[i], 8);
		fail = fail || memcmp(outplain, plain[i], 8);
		pass3 = pass3 && !fail;

		std::cout << (fail ? "FAILED   " : "passed   ");
		std::cout << '\"' << key[i] << '\"';
		for (int j=0; j<(signed int)(30-strlen(key[i])); j++)
			std::cout << ' ';
		output.Put(outplain, 8);
		std::cout << "  ";
		output.Put(out, 8);
		std::cout << std::endl;
	}
	return pass1 && pass2 && pass3;
}