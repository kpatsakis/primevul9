	TEST_METHOD(13) {
		// A detached Generation doesn't delete the associated generation
		// directory upon destruction.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		string path = generation->getPath();
		generation->detach();
		generation.reset();
		ensure_equals(getFileType(path), FT_DIRECTORY);
	}