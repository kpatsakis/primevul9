bool ValidateVMAC()
{
	std::cout << "\nVMAC validation suite running...\n";
	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/vmac.txt");
}