QPDFObjectHandle::pipeContentStreams(
    Pipeline* p, std::string const& description, std::string& all_description)
{
    std::vector<QPDFObjectHandle> streams =
        arrayOrStreamToStreamArray(
            description, all_description);
    for (std::vector<QPDFObjectHandle>::iterator iter = streams.begin();
         iter != streams.end(); ++iter)
    {
        QPDFObjectHandle stream = *iter;
        std::string og =
            QUtil::int_to_string(stream.getObjectID()) + " " +
            QUtil::int_to_string(stream.getGeneration());
        std::string description = "content stream object " + og;
        if (! stream.pipeStreamData(p, 0, qpdf_dl_specialized))
        {
            QTC::TC("qpdf", "QPDFObjectHandle errors in parsecontent");
            warn(stream.getOwningQPDF(),
                 QPDFExc(qpdf_e_damaged_pdf, "content stream",
                         description, 0,
                         "errors while decoding content stream"));
        }
    }
}