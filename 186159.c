ByteVector Ogg::XiphComment::render(bool addFramingBit) const
{
  ByteVector data;

  // Add the vendor ID length and the vendor ID.  It's important to use the
  // length of the data(String::UTF8) rather than the length of the the string
  // since this is UTF8 text and there may be more characters in the data than
  // in the UTF16 string.

  ByteVector vendorData = d->vendorID.data(String::UTF8);

  data.append(ByteVector::fromUInt(vendorData.size(), false));
  data.append(vendorData);

  // Add the number of fields.

  data.append(ByteVector::fromUInt(fieldCount(), false));

  // Iterate over the the field lists.  Our iterator returns a
  // std::pair<String, StringList> where the first String is the field name and
  // the StringList is the values associated with that field.

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it) {

    // And now iterate over the values of the current list.

    String fieldName = (*it).first;
    StringList values = (*it).second;

    StringList::ConstIterator valuesIt = values.begin();
    for(; valuesIt != values.end(); ++valuesIt) {
      ByteVector fieldData = fieldName.data(String::UTF8);
      fieldData.append('=');
      fieldData.append((*valuesIt).data(String::UTF8));

      data.append(ByteVector::fromUInt(fieldData.size(), false));
      data.append(fieldData);
    }
  }

  // Append the "framing bit".

  if(addFramingBit)
    data.append(char(1));

  return data;
}