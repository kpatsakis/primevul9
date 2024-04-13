	TEST_METHOD(12) {
		// A Generation object returned by getGeneration() doesn't delete the
		// associated generation directory upon destruction.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation0 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation1 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		
		dir.getGeneration(0).reset();
		dir.getGeneration(1).reset();
		ensure_equals(getFileType(generation0->getPath()), FT_DIRECTORY);
		ensure_equals(getFileType(generation1->getPath()), FT_DIRECTORY);
	}