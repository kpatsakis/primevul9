bool ValidateBaseCode()
{
	bool pass = true, fail;
	byte data[255];
	for (unsigned int i=0; i<255; i++)
		data[i] = byte(i);

	const char hexEncoded[] =
		"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627"
		"28292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F"
		"505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F7071727374757677"
		"78797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9F"
		"A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7"
		"C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
		"F0F1F2F3F4F5F6F7F8F9FAFBFCFDFE";
	const char base32Encoded[] =
		"AAASEA2EAWDAQCAJBIFS2DIQB6IBCESVCSKTNF22DEPBYHA7D2RUAIJCENUCKJTHFAWUWK3NFWZC8NBT"
		"GI3VIPJYG66DUQT5HS8V6R4AIFBEGTCFI3DWSUKKJPGE4VURKBIXEW4WKXMFQYC3MJPX2ZK8M7SGC2VD"
		"NTUYN35IPFXGY5DPP3ZZA6MUQP4HK7VZRB6ZW856RX9H9AEBSKB2JBNGS8EIVCWMTUG27D6SUGJJHFEX"
		"U4M3TGN4VQQJ5HW9WCS4FI7EWYVKRKFJXKX43MPQX82MDNXVYU45PP72ZG7MZRF7Z496BSQC2RCNMTYH"
		"3DE6XU8N3ZHN9WGT4MJ7JXQY49NPVYY55VQ77Z9A6HTQH3HF65V8T4RK7RYQ55ZR8D29F69W8Z5RR8H3"
		"9M7939R8";
	const char base64AndHexEncoded[] =
		"41414543417751464267634943516F4C4441304F4478415245684D554652595847426B6147787764"
		"486838674953496A4A43556D4A7967704B6973734C5334764D4445794D7A51310A4E6A63344F546F"
		"375044302B50304242516B4E4552555A4853456C4B5330784E546B395155564A5456465657563168"
		"5A576C746358563566594746695932526C5A6D646F615770720A6247317562334278636E4E306458"
		"5A3365486C3665337839666E2B4167594B44684957476834694A696F754D6A5936506B4A47536B35"
		"53566C7065596D5A71626E4A32656E3643680A6F714F6B7061616E714B6D717136797472712B7773"
		"624B7A744C573274376935757275387662362F774D484377385446787366497963724C7A4D334F7A"
		"39445230745055316462580A324E6E6132397A6433742F6734654C6A354F586D352B6A7036757673"
		"3765377638504879382F5431397666342B6672372F50332B0A";
	const char base64URLAndHexEncoded[] =
		"41414543417751464267634943516F4C4441304F4478415245684D554652595847426B6147787764"
		"486838674953496A4A43556D4A7967704B6973734C5334764D4445794D7A51314E6A63344F546F37"
		"5044302D50304242516B4E4552555A4853456C4B5330784E546B395155564A54564656575631685A"
		"576C746358563566594746695932526C5A6D646F615770726247317562334278636E4E3064585A33"
		"65486C3665337839666E2D4167594B44684957476834694A696F754D6A5936506B4A47536B355356"
		"6C7065596D5A71626E4A32656E3643686F714F6B7061616E714B6D717136797472712D7773624B7A"
		"744C573274376935757275387662365F774D484377385446787366497963724C7A4D334F7A394452"
		"3074505531646258324E6E6132397A6433745F6734654C6A354F586D352D6A703675767337653776"
		"38504879385F5431397666342D6672375F50332D";

	std::cout << "\nBase64, Base64URL, Base32 and Base16 coding validation suite running...\n\n";

	fail = !TestFilter(HexEncoder().Ref(), data, 255, (const byte *)hexEncoded, strlen(hexEncoded));
	try {HexEncoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Hex Encoding\n";
	pass = pass && !fail;

	fail = !TestFilter(HexDecoder().Ref(), (const byte *)hexEncoded, strlen(hexEncoded), data, 255);
	try {HexDecoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Hex Decoding\n";
	pass = pass && !fail;

	fail = !TestFilter(Base32Encoder().Ref(), data, 255, (const byte *)base32Encoded, strlen(base32Encoded));
	try {Base32Encoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base32 Encoding\n";
	pass = pass && !fail;

	fail = !TestFilter(Base32Decoder().Ref(), (const byte *)base32Encoded, strlen(base32Encoded), data, 255);
	try {Base32Decoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base32 Decoding\n";
	pass = pass && !fail;

	fail = !TestFilter(Base64Encoder(new HexEncoder).Ref(), data, 255, (const byte *)base64AndHexEncoded, strlen(base64AndHexEncoded));
	try {Base64Encoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base64 Encoding\n";
	pass = pass && !fail;

	fail = !TestFilter(HexDecoder(new Base64Decoder).Ref(), (const byte *)base64AndHexEncoded, strlen(base64AndHexEncoded), data, 255);
	try {Base64Decoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base64 Decoding\n";
	pass = pass && !fail;

	fail = !TestFilter(Base64URLEncoder(new HexEncoder).Ref(), data, 255, (const byte *)base64URLAndHexEncoded, strlen(base64URLAndHexEncoded));
	try {Base64URLEncoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base64 URL Encoding\n";
	pass = pass && !fail;

	fail = !TestFilter(HexDecoder(new Base64URLDecoder).Ref(), (const byte *)base64URLAndHexEncoded, strlen(base64URLAndHexEncoded), data, 255);
	try {Base64URLDecoder().IsolatedInitialize(g_nullNameValuePairs);}
	catch (const Exception&) {fail=true;}
	std::cout << (fail ? "FAILED:" : "passed:");
	std::cout << "  Base64 URL Decoding\n";
	pass = pass && !fail;

	return pass;
}