bool ValidateHmacDRBG()
{
	std::cout << "\nTesting NIST HMAC DRBGs...\n\n";
	bool pass=true, fail;

	// # CAVS 14.3
	// # DRBG800-90A information for "drbg_pr"
	// # Generated on Tue Apr 02 15:32:12 2013

	{
		// [SHA-1], [PredictionResistance = False], [EntropyInputLen = 128], [NonceLen = 64]
		// [PersonalizationStringLen = 0], [AdditionalInputLen = 0], [ReturnedBitsLen = 640]
		const byte entropy1[] = "\x79\x34\x9b\xbf\x7c\xdd\xa5\x79\x95\x57\x86\x66\x21\xc9\x13\x83";
		const byte entropy2[] = "\xc7\x21\x5b\x5b\x96\xc4\x8e\x9b\x33\x8c\x74\xe3\xe9\x9d\xfe\xdf";
		const byte nonce[] = "\x11\x46\x73\x3a\xbf\x8c\x35\xc8";

		HMAC_DRBG<SHA1, 128/8, 440/8> drbg(entropy1, 16, nonce, 8);
		drbg.IncorporateEntropy(entropy2, 16);

		SecByteBlock result(80);
		drbg.GenerateBlock(result, result.size());
		drbg.GenerateBlock(result, result.size());

		const byte expected[] = "\xc6\xa1\x6a\xb8\xd4\x20\x70\x6f\x0f\x34\xab\x7f\xec\x5a\xdc\xa9\xd8\xca\x3a\x13"
			"\x3e\x15\x9c\xa6\xac\x43\xc6\xf8\xa2\xbe\x22\x83\x4a\x4c\x0a\x0a\xff\xb1\x0d\x71"
			"\x94\xf1\xc1\xa5\xcf\x73\x22\xec\x1a\xe0\x96\x4e\xd4\xbf\x12\x27\x46\xe0\x87\xfd"
			"\xb5\xb3\xe9\x1b\x34\x93\xd5\xbb\x98\xfa\xed\x49\xe8\x5f\x13\x0f\xc8\xa4\x59\xb7";

		fail = !!memcmp(result, expected, 640/8);
		pass = !fail && pass;

		std::cout << (fail ? "FAILED   " : "passed   ") << "HMAC_DRBG SHA1/128/440 (COUNT=0, E=16, N=8)\n";
	}

	{
		// [SHA-1], [PredictionResistance = False], [EntropyInputLen = 128], [NonceLen = 64]
		// [PersonalizationStringLen = 0], [AdditionalInputLen = 0], [ReturnedBitsLen = 640]
		const byte entropy1[] = "\xee\x57\xfc\x23\x60\x0f\xb9\x02\x9a\x9e\xc6\xc8\x2e\x7b\x51\xe4";
		const byte entropy2[] = "\x84\x1d\x27\x6c\xa9\x51\x90\x61\xd9\x2d\x7d\xdf\xa6\x62\x8c\xa3";
		const byte nonce[] = "\x3e\x97\x21\xe4\x39\x3e\xf9\xad";

		HMAC_DRBG<SHA1, 128/8, 440/8> drbg(entropy1, 16, nonce, 8);
		drbg.IncorporateEntropy(entropy2, 16);

		SecByteBlock result(80);
		drbg.GenerateBlock(result, result.size());
		drbg.GenerateBlock(result, result.size());

		const byte expected[] = "\xee\x26\xa5\xc8\xef\x08\xa1\xca\x8f\x14\x15\x4d\x67\xc8\x8f\x5e\x7e\xd8\x21\x9d"
			"\x93\x1b\x98\x42\xac\x00\x39\xf2\x14\x55\x39\xf2\x14\x2b\x44\x11\x7a\x99\x8c\x22"
			"\xf5\x90\xf6\xc9\xb3\x8b\x46\x5b\x78\x3e\xcf\xf1\x3a\x77\x50\x20\x1f\x7e\xcf\x1b"
			"\x8a\xb3\x93\x60\x4c\x73\xb2\x38\x93\x36\x60\x9a\xf3\x44\x0c\xde\x43\x29\x8b\x84";

		fail = !!memcmp(result, expected, 640/8);
		pass = !fail && pass;

		std::cout << (fail ? "FAILED   " : "passed   ") << "HMAC_DRBG SHA1/128/440 (COUNT=1, E=16, N=8)\n";
	}

	// *****************************************************

	{
		// [SHA-1], [PredictionResistance = False], [EntropyInputLen = 128], [NonceLen = 64]
		// [PersonalizationStringLen = 0], [AdditionalInputLen = 16], [ReturnedBitsLen = 640]
		const byte entropy1[] = "\x7d\x70\x52\xa7\x76\xfd\x2f\xb3\xd7\x19\x1f\x73\x33\x04\xee\x8b";
		const byte entropy2[] = "\x49\x04\x7e\x87\x9d\x61\x09\x55\xee\xd9\x16\xe4\x06\x0e\x00\xc9";
		const byte nonce[] = "\xbe\x4a\x0c\xee\xdc\xa8\x02\x07";
		const byte additional1[] = "\xfd\x8b\xb3\x3a\xab\x2f\x6c\xdf\xbc\x54\x18\x11\x86\x1d\x51\x8d";
		const byte additional2[] = "\x99\xaf\xe3\x47\x54\x04\x61\xdd\xf6\xab\xeb\x49\x1e\x07\x15\xb4";
		const byte additional3[] = "\x02\xf7\x73\x48\x2d\xd7\xae\x66\xf7\x6e\x38\x15\x98\xa6\x4e\xf0";

		HMAC_DRBG<SHA1, 128/8, 440/8> drbg(entropy1, 16, nonce, 8);
		drbg.IncorporateEntropy(entropy2, 16, additional1, 16);

		SecByteBlock result(80);
		drbg.GenerateBlock(additional2, 16, result, result.size());
		drbg.GenerateBlock(additional3, 16, result, result.size());

		const byte expected[] = "\xa7\x36\x34\x38\x44\xfc\x92\x51\x13\x91\xdb\x0a\xdd\xd9\x06\x4d\xbe\xe2\x4c\x89"
			"\x76\xaa\x25\x9a\x9e\x3b\x63\x68\xaa\x6d\xe4\xc9\xbf\x3a\x0e\xff\xcd\xa9\xcb\x0e"
			"\x9d\xc3\x36\x52\xab\x58\xec\xb7\x65\x0e\xd8\x04\x67\xf7\x6a\x84\x9f\xb1\xcf\xc1"
			"\xed\x0a\x09\xf7\x15\x50\x86\x06\x4d\xb3\x24\xb1\xe1\x24\xf3\xfc\x9e\x61\x4f\xcb";

		fail = !!memcmp(result, expected, 640/8);
		pass = !fail && pass;

		std::cout << (fail ? "FAILED   " : "passed   ") << "HMAC_DRBG SHA1/128/440 (COUNT=0, E=16, N=8, A=16)\n";
	}

	{
		// [SHA-1], [PredictionResistance = False], [EntropyInputLen = 128], [NonceLen = 64]
		// [PersonalizationStringLen = 0], [AdditionalInputLen = 16], [ReturnedBitsLen = 640]
		const byte entropy1[] = "\x29\xc6\x2a\xfa\x3c\x52\x20\x8a\x3f\xde\xcb\x43\xfa\x61\x3f\x15";
		const byte entropy2[] = "\xbd\x87\xbe\x99\xd1\x84\x16\x54\x12\x31\x41\x40\xd4\x02\x71\x41";
		const byte nonce[] = "\x6c\x9e\xb5\x9a\xc3\xc2\xd4\x8b";
		const byte additional1[] = "\x43\x3d\xda\xf2\x59\xd1\x4b\xcf\x89\x76\x30\xcc\xaa\x27\x33\x8c";
		const byte additional2[] = "\x14\x11\x46\xd4\x04\xf2\x84\xc2\xd0\x2b\x6a\x10\x15\x6e\x33\x82";
		const byte additional3[] = "\xed\xc3\x43\xdb\xff\xe7\x1a\xb4\x11\x4a\xc3\x63\x9d\x44\x5b\x65";

		HMAC_DRBG<SHA1, 128/8, 440/8> drbg(entropy1, 16, nonce, 8);
		drbg.IncorporateEntropy(entropy2, 16, additional1, 16);

		SecByteBlock result(80);
		drbg.GenerateBlock(additional2, 16, result, result.size());
		drbg.GenerateBlock(additional3, 16, result, result.size());

		const byte expected[] = "\x8c\x73\x0f\x05\x26\x69\x4d\x5a\x9a\x45\xdb\xab\x05\x7a\x19\x75\x35\x7d\x65\xaf"
			"\xd3\xef\xf3\x03\x32\x0b\xd1\x40\x61\xf9\xad\x38\x75\x91\x02\xb6\xc6\x01\x16\xf6"
			"\xdb\x7a\x6e\x8e\x7a\xb9\x4c\x05\x50\x0b\x4d\x1e\x35\x7d\xf8\xe9\x57\xac\x89\x37"
			"\xb0\x5f\xb3\xd0\x80\xa0\xf9\x06\x74\xd4\x4d\xe1\xbd\x6f\x94\xd2\x95\xc4\x51\x9d";

		fail = !!memcmp(result, expected, 640/8);
		pass = !fail && pass;

		std::cout << (fail ? "FAILED   " : "passed   ") << "HMAC_DRBG SHA1/128/440 (COUNT=1, E=16, N=8, A=16)\n";
	}

	return pass;
}