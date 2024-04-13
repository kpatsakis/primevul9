Object Parser::getObj(GBool simpleOnly,
           Guchar *fileKey,
		       CryptAlgorithm encAlgorithm, int keyLength,
		       int objNum, int objGen, int recursion,
		       GBool strict) {
  Object obj;
  Stream *str;
  DecryptStream *decrypt;
  const GooString *s;
  GooString *s2;
  int c;

  // refill buffer after inline image data
  if (inlineImg == 2) {
    buf1 = lexer->getObj();
    buf2 = lexer->getObj();
    inlineImg = 0;
  }

  if (unlikely(recursion >= recursionLimit)) {
    return Object(objError);
  }

  // array
  if (!simpleOnly && buf1.isCmd("[")) {
    shift();
    obj = Object(new Array(xref));
    while (!buf1.isCmd("]") && !buf1.isEOF() && recursion + 1 < recursionLimit) {
      Object obj2 = getObj(gFalse, fileKey, encAlgorithm, keyLength, objNum, objGen, recursion + 1);
      obj.arrayAdd(std::move(obj2));
    }
    if (recursion + 1 >= recursionLimit && strict) goto err;
    if (buf1.isEOF()) {
      error(errSyntaxError, getPos(), "End of file inside array");
      if (strict) goto err;
    }
    shift();

  // dictionary or stream
  } else if (!simpleOnly && buf1.isCmd("<<")) {
    shift(objNum);
    obj = Object(new Dict(xref));
    while (!buf1.isCmd(">>") && !buf1.isEOF()) {
      if (!buf1.isName()) {
	error(errSyntaxError, getPos(), "Dictionary key must be a name object");
	if (strict) goto err;
	shift();
      } else {
	// buf1 might go away in shift(), so construct the key
	char *key = copyString(buf1.getName());
	shift();
	if (buf1.isEOF() || buf1.isError()) {
	  gfree(key);
	  if (strict && buf1.isError()) goto err;
	  break;
	}
	Object obj2 = getObj(gFalse, fileKey, encAlgorithm, keyLength, objNum, objGen, recursion + 1);
	if (unlikely(obj2.isError() && recursion + 1 >= recursionLimit)) {
	  gfree(key);
	  break;
	}
	obj.dictAdd(key, std::move(obj2));
      }
    }
    if (buf1.isEOF()) {
      error(errSyntaxError, getPos(), "End of file inside dictionary");
      if (strict) goto err;
    }
    // stream objects are not allowed inside content streams or
    // object streams
    if (buf2.isCmd("stream")) {
      if (allowStreams && (str = makeStream(std::move(obj), fileKey, encAlgorithm, keyLength,
                                            objNum, objGen, recursion + 1,
                                            strict))) {
        return Object(str);
      } else {
        return Object(objError);
      }
    } else {
      shift();
    }

  // indirect reference or integer
  } else if (buf1.isInt()) {
    const int num = buf1.getInt();
    shift();
    if (buf1.isInt() && buf2.isCmd("R")) {
      const int gen = buf1.getInt();
      shift();
      shift();
      return Object(num, gen);
    } else {
      return Object(num);
    }

  // string
  } else if (buf1.isString() && fileKey) {
    s = buf1.getString();
    s2 = new GooString();
    decrypt = new DecryptStream(new MemStream(s->getCString(), 0, s->getLength(), Object(objNull)),
				fileKey, encAlgorithm, keyLength,
				objNum, objGen);
    decrypt->reset();
    while ((c = decrypt->getChar()) != EOF) {
      s2->append((char)c);
    }
    delete decrypt;
    obj = Object(s2);
    shift();

  // simple object
  } else {
    // avoid re-allocating memory for complex objects like strings by
    // shallow copy of <buf1> to <obj> and nulling <buf1> so that
    // subsequent buf1.free() won't free this memory
    obj = std::move(buf1);
    shift();
  }

  return obj;

err:
  return Object(objError);
}