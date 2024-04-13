Error color_profile_nclx::write(StreamWriter& writer) const
{
    writer.write16(m_colour_primaries);
    writer.write16(m_transfer_characteristics);
    writer.write16(m_matrix_coefficients);
    writer.write8(m_full_range_flag ? 0x80 : 0x00);

    return Error::Ok;
}