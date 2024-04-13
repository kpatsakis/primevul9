bool ValidateGCM()
{
	std::cout << "\nAES/GCM validation suite running...\n";
	std::cout << "\n2K tables:";
	bool pass = RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/gcm.txt", MakeParameters(Name::TableSize(), (int)2048));
	std::cout << "\n64K tables:";
	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/gcm.txt", MakeParameters(Name::TableSize(), (int)64*1024)) && pass;
}