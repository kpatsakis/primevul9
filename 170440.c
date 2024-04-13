static void parseCodeDirectory(struct MACH0_(obj_t) * mo, RzBuffer *b, int offset, int datasize) {
	typedef struct __CodeDirectory {
		uint32_t magic; /* magic number (CSMAGIC_CODEDIRECTORY) */
		uint32_t length; /* total length of CodeDirectory blob */
		uint32_t version; /* compatibility version */
		uint32_t flags; /* setup and mode flags */
		uint32_t hashOffset; /* offset of hash slot element at index zero */
		uint32_t identOffset; /* offset of identifier string */
		uint32_t nSpecialSlots; /* number of special hash slots */
		uint32_t nCodeSlots; /* number of ordinary (code) hash slots */
		uint32_t codeLimit; /* limit to main image signature range */
		uint8_t hashSize; /* size of each hash in bytes */
		uint8_t hashType; /* type of hash (cdHashType* constants) */
		uint8_t platform; /* unused (must be zero) */
		uint8_t pageSize; /* log2(page size in bytes); 0 => infinite */
		uint32_t spare2; /* unused (must be zero) */
		/* followed by dynamic content as located by offset fields above */
		uint32_t scatterOffset;
		uint32_t teamIDOffset;
		uint32_t spare3;
		ut64 codeLimit64;
		ut64 execSegBase;
		ut64 execSegLimit;
		ut64 execSegFlags;
	} CS_CodeDirectory;
	ut64 off = offset;
	int psize = datasize;
	ut8 *p = calloc(1, psize);
	if (!p) {
		return;
	}
	eprintf("Offset: 0x%08" PFMT64x "\n", off);
	rz_buf_read_at(b, off, p, datasize);
	CS_CodeDirectory cscd = { 0 };
#define READFIELD(x)  cscd.x = rz_read_ble32(p + rz_offsetof(CS_CodeDirectory, x), 1)
#define READFIELD8(x) cscd.x = p[rz_offsetof(CS_CodeDirectory, x)]
	READFIELD(length);
	READFIELD(version);
	READFIELD(flags);
	READFIELD(hashOffset);
	READFIELD(identOffset);
	READFIELD(nSpecialSlots);
	READFIELD(nCodeSlots);
	READFIELD(hashSize);
	READFIELD(teamIDOffset);
	READFIELD8(hashType);
	READFIELD(pageSize);
	READFIELD(codeLimit);
	eprintf("Version: %x\n", cscd.version);
	eprintf("Flags: %x\n", cscd.flags);
	eprintf("Length: %d\n", cscd.length);
	eprintf("PageSize: %d\n", cscd.pageSize);
	eprintf("hashOffset: %d\n", cscd.hashOffset);
	eprintf("codeLimit: %d\n", cscd.codeLimit);
	eprintf("hashSize: %d\n", cscd.hashSize);
	eprintf("hashType: %d\n", cscd.hashType);
	char *identity = readString(p, cscd.identOffset, psize);
	eprintf("Identity: %s\n", identity);
	char *teamId = readString(p, cscd.teamIDOffset, psize);
	eprintf("TeamID: %s\n", teamId);
	eprintf("CodeSlots: %d\n", cscd.nCodeSlots);
	free(identity);
	free(teamId);

	const char *digest_algo = "sha1";
	switch (cscd.hashType) {
	case 0: // SHA1 == 20 bytes
	case 1: // SHA1 == 20 bytes
		digest_algo = "sha1";
		break;
	case 2: // SHA256 == 32 bytes
		digest_algo = "sha256";
		break;
	}

	// computed cdhash
	RzHashSize digest_size = 0;
	ut8 *digest = NULL;

	int fofsz = cscd.length;
	ut8 *fofbuf = calloc(fofsz, 1);
	if (fofbuf) {
		int i;
		if (rz_buf_read_at(b, off, fofbuf, fofsz) != fofsz) {
			eprintf("Invalid cdhash offset/length values\n");
			goto parseCodeDirectory_end;
		}

		digest = rz_hash_cfg_calculate_small_block(mo->hash, digest_algo, fofbuf, fofsz, &digest_size);
		if (!digest) {
			goto parseCodeDirectory_end;
		}

		eprintf("ph %s @ 0x%" PFMT64x "!%d\n", digest_algo, off, fofsz);
		eprintf("ComputedCDHash: ");
		for (i = 0; i < digest_size; i++) {
			eprintf("%02x", digest[i]);
		}
		eprintf("\n");
		RZ_FREE(digest);
		free(fofbuf);
	}
	// show and check the rest of hashes
	ut8 *hash = p + cscd.hashOffset;
	int j = 0;
	int k = 0;
	eprintf("Hashed region: 0x%08" PFMT64x " - 0x%08" PFMT64x "\n", (ut64)0, (ut64)cscd.codeLimit);
	for (j = 0; j < cscd.nCodeSlots; j++) {
		int fof = 4096 * j;
		int idx = j * digest_size;
		eprintf("0x%08" PFMT64x "  ", off + cscd.hashOffset + idx);
		for (k = 0; k < digest_size; k++) {
			eprintf("%02x", hash[idx + k]);
		}
		ut8 fofbuf[4096];
		int fofsz = RZ_MIN(sizeof(fofbuf), cscd.codeLimit - fof);
		rz_buf_read_at(b, fof, fofbuf, sizeof(fofbuf));

		digest = rz_hash_cfg_calculate_small_block(mo->hash, digest_algo, fofbuf, fofsz, &digest_size);
		if (!digest) {
			goto parseCodeDirectory_end;
		}

		if (memcmp(hash + idx, digest, digest_size)) {
			eprintf("  wx ");
			int i;
			for (i = 0; i < digest_size; i++) {
				eprintf("%02x", digest[i]);
			}
		} else {
			eprintf("  OK");
		}
		eprintf("\n");
		free(digest);
	}

parseCodeDirectory_end:
	free(p);
}