QPDFObjectHandle::objectWarning(std::string const& warning)
{
    QPDF* context = 0;
    std::string description;
    if (this->m->obj->getDescription(context, description))
    {
        warn(context,
             QPDFExc(
                 qpdf_e_damaged_pdf,
                 "", description, 0,
                 warning));
    }
    else
    {
        throw std::logic_error(warning);
    }
}