	void IptcData::printStructure(std::ostream& out, const byte* bytes,const size_t size,uint32_t depth)
	{
		uint32_t i     = 0 ;
		while  ( i < size-3 && bytes[i] != 0x1c ) i++;
		depth++;
		out << indent(depth) << "Record | DataSet | Name                     | Length | Data" << std::endl;
		while ( bytes[i] == 0x1c && i < size-3 ) {
			char buff[100];
			uint16_t record  = bytes[i+1];
			uint16_t dataset = bytes[i+2];
			uint16_t len     = getUShort(bytes+i+3,bigEndian);
			sprintf(buff,"  %6d | %7d | %-24s | %6d | ",record,dataset, Exiv2::IptcDataSets::dataSetName(dataset,record).c_str(), len);

			out << buff << Internal::binaryToString(bytes,(len>40?40:len),i+5) << (len>40?"...":"") << std::endl;
			i += 5 + len;
		}
		depth--;
	}