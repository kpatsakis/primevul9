bool ValidateRC5()
{
	std::cout << "\nRC5 validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	RC5Encryption enc;  // 0 to 2040-bits (255-bytes)
	pass1 = RC5Encryption::DEFAULT_KEYLENGTH ==  16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(0) == 0 && pass1;
	pass1 = enc.StaticGetValidKeyLength(254) == 254 && pass1;
	pass1 = enc.StaticGetValidKeyLength(255) == 255 && pass1;
	pass1 = enc.StaticGetValidKeyLength(256) == 255 && pass1;
	pass1 = enc.StaticGetValidKeyLength(0) == enc.MinKeyLength() && pass1;
	pass1 = enc.StaticGetValidKeyLength(SIZE_MAX) == enc.MaxKeyLength() && pass1;

	RC5Decryption dec;
	pass2 = RC5Decryption::DEFAULT_KEYLENGTH ==  16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(0) == 0 && pass2;
	pass2 = dec.StaticGetValidKeyLength(254) == 254 && pass2;
	pass2 = dec.StaticGetValidKeyLength(255) == 255 && pass2;
	pass2 = dec.StaticGetValidKeyLength(256) == 255 && pass2;
	pass2 = dec.StaticGetValidKeyLength(0) == dec.MinKeyLength() && pass2;
	pass2 = dec.StaticGetValidKeyLength(SIZE_MAX) == dec.MaxKeyLength() && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/rc5val.dat", true, new HexDecoder);
	return BlockTransformationTest(VariableRoundsCipherFactory<RC5Encryption, RC5Decryption>(16, 12), valdata) && pass1 && pass2;
}