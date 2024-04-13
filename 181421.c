bool ValidateCCM()
{
	std::cout << "\nAES/CCM validation suite running...\n";
	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/ccm.txt");
}