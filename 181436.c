bool ValidateSHARK()
{
	std::cout << "\nSHARK validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	SHARKEncryption enc;  // 128-bit only
	pass1 = SHARKEncryption::KEYLENGTH ==  16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(15) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(17) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(32) == 16 && pass1;

	SHARKDecryption dec;  // 128-bit only
	pass2 = SHARKDecryption::KEYLENGTH ==  16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(15) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(17) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(32) == 16 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/sharkval.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<SHARKEncryption, SHARKDecryption>(), valdata) && pass1 && pass2;
}