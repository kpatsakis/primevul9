bool BlockTransformationTest(const CipherFactory &cg, BufferedTransformation &valdata, unsigned int tuples = 0xffff)
{
	HexEncoder output(new FileSink(std::cout));
	SecByteBlock plain(cg.BlockSize()), cipher(cg.BlockSize()), out(cg.BlockSize()), outplain(cg.BlockSize());
	SecByteBlock key(cg.KeyLength());
	bool pass=true, fail;

	while (valdata.MaxRetrievable() && tuples--)
	{
		valdata.Get(key, cg.KeyLength());
		valdata.Get(plain, cg.BlockSize());
		valdata.Get(cipher, cg.BlockSize());

		member_ptr<BlockTransformation> transE(cg.NewEncryption(key));
		transE->ProcessBlock(plain, out);
		fail = memcmp(out, cipher, cg.BlockSize()) != 0;

		member_ptr<BlockTransformation> transD(cg.NewDecryption(key));
		transD->ProcessBlock(out, outplain);
		fail=fail || memcmp(outplain, plain, cg.BlockSize());

		pass = pass && !fail;

		std::cout << (fail ? "FAILED   " : "passed   ");
		output.Put(key, cg.KeyLength());
		std::cout << "   ";
		output.Put(outplain, cg.BlockSize());
		std::cout << "   ";
		output.Put(out, cg.BlockSize());
		std::cout << std::endl;
	}
	return pass;
}