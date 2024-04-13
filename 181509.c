bool ValidateRC2()
{
	std::cout << "\nRC2 validation suite running...\n\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/rc2val.dat", true, new HexDecoder);
	HexEncoder output(new FileSink(std::cout));
	SecByteBlock plain(RC2Encryption::BLOCKSIZE), cipher(RC2Encryption::BLOCKSIZE), out(RC2Encryption::BLOCKSIZE), outplain(RC2Encryption::BLOCKSIZE);
	SecByteBlock key(128);
	bool pass=true, fail;

	while (valdata.MaxRetrievable())
	{
		byte keyLen, effectiveLen;

		valdata.Get(keyLen);
		valdata.Get(effectiveLen);
		valdata.Get(key, keyLen);
		valdata.Get(plain, RC2Encryption::BLOCKSIZE);
		valdata.Get(cipher, RC2Encryption::BLOCKSIZE);

		member_ptr<BlockTransformation> transE(new RC2Encryption(key, keyLen, effectiveLen));
		transE->ProcessBlock(plain, out);
		fail = memcmp(out, cipher, RC2Encryption::BLOCKSIZE) != 0;

		member_ptr<BlockTransformation> transD(new RC2Decryption(key, keyLen, effectiveLen));
		transD->ProcessBlock(out, outplain);
		fail=fail || memcmp(outplain, plain, RC2Encryption::BLOCKSIZE);

		pass = pass && !fail;

		std::cout << (fail ? "FAILED   " : "passed   ");
		output.Put(key, keyLen);
		std::cout << "   ";
		output.Put(outplain, RC2Encryption::BLOCKSIZE);
		std::cout << "   ";
		output.Put(out, RC2Encryption::BLOCKSIZE);
		std::cout << std::endl;
	}
	return pass;
}