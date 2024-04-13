bool ValidateThreeWay()
{
	std::cout << "\n3-WAY validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	ThreeWayEncryption enc;  // 96-bit only
	pass1 = ThreeWayEncryption::KEYLENGTH ==  12 && pass1;
	pass1 = enc.StaticGetValidKeyLength(8) == 12 && pass1;
	pass1 = enc.StaticGetValidKeyLength(12) == 12 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 12 && pass1;

	ThreeWayDecryption dec;  // 96-bit only
	pass2 = ThreeWayDecryption::KEYLENGTH ==  12 && pass2;
	pass2 = dec.StaticGetValidKeyLength(8) == 12 && pass2;
	pass2 = dec.StaticGetValidKeyLength(12) == 12 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 12 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/3wayval.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<ThreeWayEncryption, ThreeWayDecryption>(), valdata) && pass1 && pass2;
}