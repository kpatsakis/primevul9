bool ValidateCipherModes()
{
	std::cout << "\nTesting DES modes...\n\n";
	const byte key[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
	const byte iv[] = {0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef};
	const byte plain[] = {	// "Now is the time for all " without tailing 0
		0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
		0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
		0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20};
	DESEncryption desE(key);
	DESDecryption desD(key);
	bool pass=true, fail;

	{
		// from FIPS 81
		const byte encrypted[] = {
			0x3f, 0xa4, 0x0e, 0x8a, 0x98, 0x4d, 0x48, 0x15,
			0x6a, 0x27, 0x17, 0x87, 0xab, 0x88, 0x83, 0xf9,
			0x89, 0x3d, 0x51, 0xec, 0x4b, 0x56, 0x3b, 0x53};

		ECB_Mode_ExternalCipher::Encryption modeE(desE);
		fail = !TestFilter(StreamTransformationFilter(modeE, NULLPTR, StreamTransformationFilter::NO_PADDING).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "ECB encryption" << std::endl;

		ECB_Mode_ExternalCipher::Decryption modeD(desD);
		fail = !TestFilter(StreamTransformationFilter(modeD, NULLPTR, StreamTransformationFilter::NO_PADDING).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "ECB decryption" << std::endl;
	}
	{
		// from FIPS 81
		const byte encrypted[] = {
			0xE5, 0xC7, 0xCD, 0xDE, 0x87, 0x2B, 0xF2, 0x7C,
			0x43, 0xE9, 0x34, 0x00, 0x8C, 0x38, 0x9C, 0x0F,
			0x68, 0x37, 0x88, 0x49, 0x9A, 0x7C, 0x05, 0xF6};

		CBC_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE, NULLPTR, StreamTransformationFilter::NO_PADDING).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with no padding" << std::endl;

		CBC_Mode_ExternalCipher::Decryption modeD(desD, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD, NULLPTR, StreamTransformationFilter::NO_PADDING).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with no padding" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC mode IV generation" << std::endl;
	}
	{
		// generated with Crypto++, matches FIPS 81
		// but has extra 8 bytes as result of padding
		const byte encrypted[] = {
			0xE5, 0xC7, 0xCD, 0xDE, 0x87, 0x2B, 0xF2, 0x7C,
			0x43, 0xE9, 0x34, 0x00, 0x8C, 0x38, 0x9C, 0x0F,
			0x68, 0x37, 0x88, 0x49, 0x9A, 0x7C, 0x05, 0xF6,
			0x62, 0xC1, 0x6A, 0x27, 0xE4, 0xFC, 0xF2, 0x77};

		CBC_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with PKCS #7 padding" << std::endl;

		CBC_Mode_ExternalCipher::Decryption modeD(desD, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with PKCS #7 padding" << std::endl;
	}
	{
		// generated with Crypto++ 5.2, matches FIPS 81
		// but has extra 8 bytes as result of padding
		const byte encrypted[] = {
			0xE5, 0xC7, 0xCD, 0xDE, 0x87, 0x2B, 0xF2, 0x7C,
			0x43, 0xE9, 0x34, 0x00, 0x8C, 0x38, 0x9C, 0x0F,
			0x68, 0x37, 0x88, 0x49, 0x9A, 0x7C, 0x05, 0xF6,
			0xcf, 0xb7, 0xc7, 0x64, 0x0e, 0x7c, 0xd9, 0xa7};

		CBC_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE, NULLPTR, StreamTransformationFilter::ONE_AND_ZEROS_PADDING).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with one-and-zeros padding" << std::endl;

		CBC_Mode_ExternalCipher::Decryption modeD(desD, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD, NULLPTR, StreamTransformationFilter::ONE_AND_ZEROS_PADDING).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with one-and-zeros padding" << std::endl;
	}
	{
		const byte plain_1[] = {'a', 0, 0, 0, 0, 0, 0, 0};
		// generated with Crypto++
		const byte encrypted[] = {
			0x9B, 0x47, 0x57, 0x59, 0xD6, 0x9C, 0xF6, 0xD0};

		CBC_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE, NULLPTR, StreamTransformationFilter::ZEROS_PADDING).Ref(),
			plain_1, 1, encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with zeros padding" << std::endl;

		CBC_Mode_ExternalCipher::Decryption modeD(desD, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD, NULLPTR, StreamTransformationFilter::ZEROS_PADDING).Ref(),
			encrypted, sizeof(encrypted), plain_1, sizeof(plain_1));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with zeros padding" << std::endl;
	}
	{
		// generated with Crypto++, matches FIPS 81
		// but with last two blocks swapped as result of CTS
		const byte encrypted[] = {
			0xE5, 0xC7, 0xCD, 0xDE, 0x87, 0x2B, 0xF2, 0x7C,
			0x68, 0x37, 0x88, 0x49, 0x9A, 0x7C, 0x05, 0xF6,
			0x43, 0xE9, 0x34, 0x00, 0x8C, 0x38, 0x9C, 0x0F};

		CBC_CTS_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with ciphertext stealing (CTS)" << std::endl;

		CBC_CTS_Mode_ExternalCipher::Decryption modeD(desD, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with ciphertext stealing (CTS)" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC CTS IV generation" << std::endl;
	}
	{
		// generated with Crypto++
		const byte decryptionIV[] = {0x4D, 0xD0, 0xAC, 0x8F, 0x47, 0xCF, 0x79, 0xCE};
		const byte encrypted[] = {0x12, 0x34, 0x56};

		byte stolenIV[8];

		CBC_CTS_Mode_ExternalCipher::Encryption modeE(desE, iv);
		modeE.SetStolenIV(stolenIV);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, 3, encrypted, sizeof(encrypted));
		fail = memcmp(stolenIV, decryptionIV, 8) != 0 || fail;
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC encryption with ciphertext and IV stealing" << std::endl;

		CBC_CTS_Mode_ExternalCipher::Decryption modeD(desD, stolenIV);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, 3);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC decryption with ciphertext and IV stealing" << std::endl;
	}
	{
		const byte encrypted[] = {	// from FIPS 81
			0xF3,0x09,0x62,0x49,0xC7,0xF4,0x6E,0x51,
			0xA6,0x9E,0x83,0x9B,0x1A,0x92,0xF7,0x84,
			0x03,0x46,0x71,0x33,0x89,0x8E,0xA6,0x22};

		CFB_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB encryption" << std::endl;

		CFB_Mode_ExternalCipher::Decryption modeD(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB decryption" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB mode IV generation" << std::endl;
	}
	{
		const byte plain_2[] = {	// "Now is the." without tailing 0
			0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,0x68,0x65};
		const byte encrypted[] = {	// from FIPS 81
			0xf3,0x1f,0xda,0x07,0x01,0x14,0x62,0xee,0x18,0x7f};

		CFB_Mode_ExternalCipher::Encryption modeE(desE, iv, 1);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain_2, sizeof(plain_2), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB (8-bit feedback) encryption" << std::endl;

		CFB_Mode_ExternalCipher::Decryption modeD(desE, iv, 1);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain_2, sizeof(plain_2));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB (8-bit feedback) decryption" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CFB (8-bit feedback) IV generation" << std::endl;
	}
	{
		const byte encrypted[] = {	// from Eric Young's libdes
			0xf3,0x09,0x62,0x49,0xc7,0xf4,0x6e,0x51,
			0x35,0xf2,0x4a,0x24,0x2e,0xeb,0x3d,0x3f,
			0x3d,0x6d,0x5b,0xe3,0x25,0x5a,0xf8,0xc3};

		OFB_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "OFB encryption" << std::endl;

		OFB_Mode_ExternalCipher::Decryption modeD(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "OFB decryption" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "OFB IV generation" << std::endl;
	}
	{
		const byte encrypted[] = {	// generated with Crypto++
			0xF3, 0x09, 0x62, 0x49, 0xC7, 0xF4, 0x6E, 0x51,
			0x16, 0x3A, 0x8C, 0xA0, 0xFF, 0xC9, 0x4C, 0x27,
			0xFA, 0x2F, 0x80, 0xF4, 0x80, 0xB8, 0x6F, 0x75};

		CTR_Mode_ExternalCipher::Encryption modeE(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeE).Ref(),
			plain, sizeof(plain), encrypted, sizeof(encrypted));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "Counter Mode encryption" << std::endl;

		CTR_Mode_ExternalCipher::Decryption modeD(desE, iv);
		fail = !TestFilter(StreamTransformationFilter(modeD).Ref(),
			encrypted, sizeof(encrypted), plain, sizeof(plain));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "Counter Mode decryption" << std::endl;

		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "Counter Mode IV generation" << std::endl;
	}
	{
		const byte plain_3[] = {	// "7654321 Now is the time for "
			0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x20,
			0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
			0x68, 0x65, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20,
			0x66, 0x6f, 0x72, 0x20};
		const byte mac1[] = {	// from FIPS 113
			0xf1, 0xd3, 0x0f, 0x68, 0x49, 0x31, 0x2c, 0xa4};
		const byte mac2[] = {	// generated with Crypto++
			0x35, 0x80, 0xC5, 0xC4, 0x6B, 0x81, 0x24, 0xE2};

		CBC_MAC<DES> cbcmac(key);
		HashFilter cbcmacFilter(cbcmac);
		fail = !TestFilter(cbcmacFilter, plain_3, sizeof(plain_3), mac1, sizeof(mac1));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "CBC MAC" << std::endl;

		DMAC<DES> dmac(key);
		HashFilter dmacFilter(dmac);
		fail = !TestFilter(dmacFilter, plain_3, sizeof(plain_3), mac2, sizeof(mac2));
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "DMAC" << std::endl;
	}
	{
		CTR_Mode<AES>::Encryption modeE(plain, 16, plain);
		CTR_Mode<AES>::Decryption modeD(plain, 16, plain);
		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "AES CTR Mode" << std::endl;
	}
	{
		OFB_Mode<AES>::Encryption modeE(plain, 16, plain);
		OFB_Mode<AES>::Decryption modeD(plain, 16, plain);
		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "AES OFB Mode" << std::endl;
	}
	{
		CFB_Mode<AES>::Encryption modeE(plain, 16, plain);
		CFB_Mode<AES>::Decryption modeD(plain, 16, plain);
		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "AES CFB Mode" << std::endl;
	}
	{
		CBC_Mode<AES>::Encryption modeE(plain, 16, plain);
		CBC_Mode<AES>::Decryption modeD(plain, 16, plain);
		fail = !TestModeIV(modeE, modeD);
		pass = pass && !fail;
		std::cout << (fail ? "FAILED   " : "passed   ") << "AES CBC Mode" << std::endl;
	}

	return pass;
}