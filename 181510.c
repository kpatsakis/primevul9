bool ValidateTwofish()
{
	std::cout << "\nTwofish validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true;

	TwofishEncryption enc;
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(24) == 24 && pass1;
	pass1 = enc.StaticGetValidKeyLength(32) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(64) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(128) == 32 && pass1;

	TwofishDecryption dec;
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(24) == 24 && pass2;
	pass2 = dec.StaticGetValidKeyLength(32) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(64) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(128) == 32 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/twofishv.dat", true, new HexDecoder);
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<TwofishEncryption, TwofishDecryption>(16), valdata, 4) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<TwofishEncryption, TwofishDecryption>(24), valdata, 3) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<TwofishEncryption, TwofishDecryption>(32), valdata, 2) && pass3;
	return pass1 && pass2 && pass3;
}