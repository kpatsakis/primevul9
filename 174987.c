Error Box::read(BitstreamRange& range, std::shared_ptr<heif::Box>* result)
{
  BoxHeader hdr;
  Error err = hdr.parse(range);
  if (err) {
    return err;
  }

  if (range.error()) {
    return range.get_error();
  }

  std::shared_ptr<Box> box;

  switch (hdr.get_short_type()) {
  case fourcc("ftyp"):
    box = std::make_shared<Box_ftyp>(hdr);
    break;

  case fourcc("meta"):
    box = std::make_shared<Box_meta>(hdr);
    break;

  case fourcc("hdlr"):
    box = std::make_shared<Box_hdlr>(hdr);
    break;

  case fourcc("pitm"):
    box = std::make_shared<Box_pitm>(hdr);
    break;

  case fourcc("iloc"):
    box = std::make_shared<Box_iloc>(hdr);
    break;

  case fourcc("iinf"):
    box = std::make_shared<Box_iinf>(hdr);
    break;

  case fourcc("infe"):
    box = std::make_shared<Box_infe>(hdr);
    break;

  case fourcc("iprp"):
    box = std::make_shared<Box_iprp>(hdr);
    break;

  case fourcc("ipco"):
    box = std::make_shared<Box_ipco>(hdr);
    break;

  case fourcc("ipma"):
    box = std::make_shared<Box_ipma>(hdr);
    break;

  case fourcc("ispe"):
    box = std::make_shared<Box_ispe>(hdr);
    break;

  case fourcc("auxC"):
    box = std::make_shared<Box_auxC>(hdr);
    break;

  case fourcc("irot"):
    box = std::make_shared<Box_irot>(hdr);
    break;

  case fourcc("imir"):
    box = std::make_shared<Box_imir>(hdr);
    break;

  case fourcc("clap"):
    box = std::make_shared<Box_clap>(hdr);
    break;

  case fourcc("iref"):
    box = std::make_shared<Box_iref>(hdr);
    break;

  case fourcc("hvcC"):
    box = std::make_shared<Box_hvcC>(hdr);
    break;

  case fourcc("idat"):
    box = std::make_shared<Box_idat>(hdr);
    break;

  case fourcc("grpl"):
    box = std::make_shared<Box_grpl>(hdr);
    break;

  case fourcc("dinf"):
    box = std::make_shared<Box_dinf>(hdr);
    break;

  case fourcc("dref"):
    box = std::make_shared<Box_dref>(hdr);
    break;

  case fourcc("url "):
    box = std::make_shared<Box_url>(hdr);
    break;

  case fourcc("colr"):
    box = std::make_shared<Box_colr>(hdr);
    break;

  case fourcc("pixi"):
    box = std::make_shared<Box_pixi>(hdr);
    break;

  default:
    box = std::make_shared<Box>(hdr);
    break;
  }

  if (hdr.get_box_size() < hdr.get_header_size()) {
    std::stringstream sstr;
    sstr << "Box size (" << hdr.get_box_size() << " bytes) smaller than header size ("
         << hdr.get_header_size() << " bytes)";

    // Sanity check.
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_box_size,
                 sstr.str());
  }


  if (range.get_nesting_level() > MAX_BOX_NESTING_LEVEL) {
    return Error(heif_error_Memory_allocation_error,
                 heif_suberror_Security_limit_exceeded,
                 "Security limit for maximum nesting of boxes has been exceeded");
  }


  auto status = range.wait_for_available_bytes( hdr.get_box_size() - hdr.get_header_size() );
  if (status != StreamReader::size_reached) {
    // TODO: return recoverable error at timeout
    return Error(heif_error_Invalid_input,
                 heif_suberror_End_of_data);
  }


  // Security check: make sure that box size does not exceed int64 size.

  if (hdr.get_box_size() > std::numeric_limits<int64_t>::max()) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_box_size);
  }

  int64_t box_size = static_cast<int64_t>(hdr.get_box_size());
  int64_t box_size_without_header = box_size - hdr.get_header_size();

  // Box size may not be larger than remaining bytes in parent box.

  if (range.get_remaining_bytes() < box_size_without_header) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_box_size);
  }


  // Create child bitstream range and read box from that range.

  BitstreamRange boxrange(range.get_istream(),
                          box_size_without_header,
                          &range);

  err = box->parse(boxrange);
  if (err == Error::Ok) {
    *result = std::move(box);
  }

  boxrange.skip_to_end_of_box();

  return err;
}