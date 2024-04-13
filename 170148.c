	~ServerInstanceDir() {
		if (owner) {
			GenerationPtr newestGeneration;
			try {
				newestGeneration = getNewestGeneration();
			} catch (const FileSystemException &e) {
				if (e.code() == ENOENT) {
					return;
				} else {
					throw;
				}
			}
			if (newestGeneration == NULL) {
				removeDirTree(path);
			}
		}
	}