bool ValidateDES()
{
	std::cout << "\nDES validation suite running...\n\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/descert.dat", true, new HexDecoder);
	bool pass = BlockTransformationTest(FixedRoundsCipherFactory<DESEncryption, DESDecryption>(), valdata);

	std::cout << "\nTesting EDE2, EDE3, and XEX3 variants...\n\n";

	FileSource valdata1(CRYPTOPP_DATA_DIR "TestData/3desval.dat", true, new HexDecoder);
	pass = BlockTransformationTest(FixedRoundsCipherFactory<DES_EDE2_Encryption, DES_EDE2_Decryption>(), valdata1, 1) && pass;
	pass = BlockTransformationTest(FixedRoundsCipherFactory<DES_EDE3_Encryption, DES_EDE3_Decryption>(), valdata1, 1) && pass;
	pass = BlockTransformationTest(FixedRoundsCipherFactory<DES_XEX3_Encryption, DES_XEX3_Decryption>(), valdata1, 1) && pass;

	return pass;
}