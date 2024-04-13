QPDFObjectHandle::getPageImages()
{
    // Note: this code doesn't handle inherited resources.  If this
    // page dictionary doesn't have a /Resources key or has one whose
    // value is null or an empty dictionary, you are supposed to walk
    // up the page tree until you find a /Resources dictionary.  As of
    // this writing, I don't have any test files that use inherited
    // resources, and hand-generating one won't be a good test because
    // any mistakes in my understanding would be present in both the
    // code and the test file.

    // NOTE: If support of inherited resources (see above comment) is
    // implemented, edit comment in QPDFObjectHandle.hh for this
    // function.  Also remove call to pushInheritedAttributesToPage
    // from qpdf.cc when show_page_images is true.

    std::map<std::string, QPDFObjectHandle> result;
    if (this->hasKey("/Resources"))
    {
	QPDFObjectHandle resources = this->getKey("/Resources");
	if (resources.hasKey("/XObject"))
	{
	    QPDFObjectHandle xobject = resources.getKey("/XObject");
	    std::set<std::string> keys = xobject.getKeys();
	    for (std::set<std::string>::iterator iter = keys.begin();
		 iter != keys.end(); ++iter)
	    {
		std::string key = (*iter);
		QPDFObjectHandle value = xobject.getKey(key);
		if (value.isStream())
		{
		    QPDFObjectHandle dict = value.getDict();
		    if (dict.hasKey("/Subtype") &&
			(dict.getKey("/Subtype").getName() == "/Image") &&
			(! dict.hasKey("/ImageMask")))
		    {
			result[key] = value;
		    }
		}
	    }
	}
    }

    return result;
}