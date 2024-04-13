	GenerationPtr getGeneration(unsigned int number) const {
		// Must not used make_shared() here because Watchdog.cpp
		// deletes the raw pointer in cleanupAgentsInBackground().
		return ptr(new Generation(path, number));
	}