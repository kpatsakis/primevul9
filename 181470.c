bool ValidateGOST()
{
	std::cout << "\nGOST validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	GOSTEncryption enc;  // 256-bit only
	pass1 = GOSTEncryption::KEYLENGTH ==  32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(24) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(32) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(40) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(64) == 32 && pass1;

	GOSTDecryption dec;  // 256-bit only
	pass2 = GOSTDecryption::KEYLENGTH ==  32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(24) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(32) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(40) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(64) == 32 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/gostval.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<GOSTEncryption, GOSTDecryption>(), valdata) && pass1 && pass2;
}