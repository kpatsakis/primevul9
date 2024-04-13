		~Generation() {
			if (owner) {
				removeDirTree(path);
			}
		}