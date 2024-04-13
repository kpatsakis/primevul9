bool ValidateSerpent()
{
	std::cout << "\nSerpent validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true;

	SerpentEncryption enc;
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(24) == 24 && pass1;
	pass1 = enc.StaticGetValidKeyLength(32) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(64) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(128) == 32 && pass1;

	SerpentDecryption dec;
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(24) == 24 && pass2;
	pass2 = dec.StaticGetValidKeyLength(32) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(64) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(128) == 32 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/serpentv.dat", true, new HexDecoder);
	bool pass = true;
	pass = BlockTransformationTest(FixedRoundsCipherFactory<SerpentEncryption, SerpentDecryption>(16), valdata, 5) && pass;
	pass = BlockTransformationTest(FixedRoundsCipherFactory<SerpentEncryption, SerpentDecryption>(24), valdata, 4) && pass;
	pass = BlockTransformationTest(FixedRoundsCipherFactory<SerpentEncryption, SerpentDecryption>(32), valdata, 3) && pass;
	return pass1 && pass2 && pass3;
}