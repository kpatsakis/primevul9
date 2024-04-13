bool ValidateCAST()
{
	std::cout << "\nCAST-128 validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true;

	CAST128Encryption enc1;  // 40 to 128-bits (5 to 16-bytes)
	pass1 = CAST128Encryption::DEFAULT_KEYLENGTH ==  16 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(4) == 5 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(5) == 5 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(15) == 15 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc1.StaticGetValidKeyLength(17) == 16 && pass1;

	CAST128Decryption dec1;  // 40 to 128-bits (5 to 16-bytes)
	pass2 = CAST128Decryption::DEFAULT_KEYLENGTH ==  16 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(4) == 5 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(5) == 5 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(15) == 15 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec1.StaticGetValidKeyLength(17) == 16 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource val128(CRYPTOPP_DATA_DIR "TestData/cast128v.dat", true, new HexDecoder);
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<CAST128Encryption, CAST128Decryption>(16), val128, 1) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<CAST128Encryption, CAST128Decryption>(10), val128, 1) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<CAST128Encryption, CAST128Decryption>(5), val128, 1) && pass3;

	std::cout << "\nCAST-256 validation suite running...\n\n";
	bool pass4 = true, pass5 = true, pass6 = true;

	CAST256Encryption enc2;  // 128, 160, 192, 224, or 256-bits (16 to 32-bytes, step 4)
	pass1 = CAST128Encryption::DEFAULT_KEYLENGTH ==  16 && pass1;
	pass4 = enc2.StaticGetValidKeyLength(15) == 16 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(16) == 16 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(17) == 20 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(20) == 20 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(24) == 24 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(28) == 28 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(31) == 32 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(32) == 32 && pass4;
	pass4 = enc2.StaticGetValidKeyLength(33) == 32 && pass4;

	CAST256Decryption dec2;  // 128, 160, 192, 224, or 256-bits (16 to 32-bytes, step 4)
	pass2 = CAST256Decryption::DEFAULT_KEYLENGTH ==  16 && pass2;
	pass5 = dec2.StaticGetValidKeyLength(15) == 16 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(16) == 16 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(17) == 20 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(20) == 20 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(24) == 24 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(28) == 28 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(31) == 32 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(32) == 32 && pass5;
	pass5 = dec2.StaticGetValidKeyLength(33) == 32 && pass5;
	std::cout << (pass4 && pass5 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource val256(CRYPTOPP_DATA_DIR "TestData/cast256v.dat", true, new HexDecoder);
	pass6 = BlockTransformationTest(FixedRoundsCipherFactory<CAST256Encryption, CAST256Decryption>(16), val256, 1) && pass6;
	pass6 = BlockTransformationTest(FixedRoundsCipherFactory<CAST256Encryption, CAST256Decryption>(24), val256, 1) && pass6;
	pass6 = BlockTransformationTest(FixedRoundsCipherFactory<CAST256Encryption, CAST256Decryption>(32), val256, 1) && pass6;

	return pass1 && pass2 && pass3 && pass4 && pass5 && pass6;
}