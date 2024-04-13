Object getFileSpecNameForPlatform (const Object *fileSpec)
{
  if (fileSpec->isString()) {
    return fileSpec->copy();
  }

  Object fileName;
  if (fileSpec->isDict()) {
    fileName = fileSpec->dictLookup("UF");
    if (!fileName.isString ()) {
      fileName = fileSpec->dictLookup("F");
      if (!fileName.isString ()) {
#ifdef _WIN32
	const char *platform = "DOS";
#else
	const char *platform = "Unix";
#endif
        fileName = fileSpec->dictLookup(platform);
	if (!fileName.isString ()) {
	  error(errSyntaxError, -1, "Illegal file spec");
	  return Object();
	}
      }
    }
  } else {
    error(errSyntaxError, -1, "Illegal file spec");
    return Object();
  }

  // system-dependent path manipulation
#ifdef _WIN32
  int i, j;
  GooString *name = fileName.getString()->copy();
  // "//...."             --> "\...."
  // "/x/...."            --> "x:\...."
  // "/server/share/...." --> "\\server\share\...."
  // convert escaped slashes to slashes and unescaped slashes to backslashes
  i = 0;
  if (name->getChar(0) == '/') {
    if (name->getLength() >= 2 && name->getChar(1) == '/') {
      name->del(0);
      i = 0;
    } else if (name->getLength() >= 2 &&
	       ((name->getChar(1) >= 'a' && name->getChar(1) <= 'z') ||
		(name->getChar(1) >= 'A' && name->getChar(1) <= 'Z')) &&
	       (name->getLength() == 2 || name->getChar(2) == '/')) {
      name->setChar(0, name->getChar(1));
      name->setChar(1, ':');
      i = 2;
    } else {
      for (j = 2; j < name->getLength(); ++j) {
        if (name->getChar(j-1) != '\\' &&
	    name->getChar(j) == '/') {
	  break;
	}
      }
      if (j < name->getLength()) {
        name->setChar(0, '\\');
	name->insert(0, '\\');
	i = 2;
      }
    }
  }
  for (; i < name->getLength(); ++i) {
    if (name->getChar(i) == '/') {
      name->setChar(i, '\\');
    } else if (name->getChar(i) == '\\' &&
	       i+1 < name->getLength() &&
	       name->getChar(i+1) == '/') {
      name->del(i);
    }
  }
  fileName = Object(name);
#endif /* _WIN32 */

  return fileName;
}