	TEST_METHOD(7) {
		// A Generation object returned by newGeneration() deletes the associated
		// generation directory upon destruction.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation = dir.newGeneration(true,
			"nobody", nobodyGroup, 0, 0);
		string path = generation->getPath();
		generation.reset();
		ensure_equals(getFileType(path), FT_NONEXISTANT);
	}