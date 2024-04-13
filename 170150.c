	TEST_METHOD(10) {
		// A Generation object returned by getNewestGeneration() doesn't delete
		// the associated generation directory upon destruction.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr newestGeneration = dir.getNewestGeneration();
		newestGeneration.reset();
		ensure_equals(getFileType(generation->getPath()), FT_DIRECTORY);
	}