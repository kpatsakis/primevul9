bool ValidateSAFER()
{
	std::cout << "\nSAFER validation suite running...\n\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/saferval.dat", true, new HexDecoder);
	bool pass = true;
	pass = BlockTransformationTest(VariableRoundsCipherFactory<SAFER_K_Encryption, SAFER_K_Decryption>(8,6), valdata, 4) && pass;
	pass = BlockTransformationTest(VariableRoundsCipherFactory<SAFER_K_Encryption, SAFER_K_Decryption>(16,12), valdata, 4) && pass;
	pass = BlockTransformationTest(VariableRoundsCipherFactory<SAFER_SK_Encryption, SAFER_SK_Decryption>(8,6), valdata, 4) && pass;
	pass = BlockTransformationTest(VariableRoundsCipherFactory<SAFER_SK_Encryption, SAFER_SK_Decryption>(16,10), valdata, 4) && pass;
	return pass;
}