setMasksDefault (unsigned short        biBitCnt,
                 Bitmap_Channel *masks)
{
  switch (biBitCnt)
    {
    case 32:
      masks[0].mask      = 0x00ff0000;
      masks[0].shiftin   = 16;
      masks[0].max_value = (float)255.0;
      masks[1].mask      = 0x0000ff00;
      masks[1].shiftin   = 8;
      masks[1].max_value = (float)255.0;
      masks[2].mask      = 0x000000ff;
      masks[2].shiftin   = 0;
      masks[2].max_value = (float)255.0;
      masks[3].mask      = 0x00000000;
      masks[3].shiftin   = 0;
      masks[3].max_value = (float)0.0;
      break;

    case 24:
      masks[0].mask      = 0xff0000;
      masks[0].shiftin   = 16;
      masks[0].max_value = (float)255.0;
      masks[1].mask      = 0x00ff00;
      masks[1].shiftin   = 8;
      masks[1].max_value = (float)255.0;
      masks[2].mask      = 0x0000ff;
      masks[2].shiftin   = 0;
      masks[2].max_value = (float)255.0;
      masks[3].mask      = 0x0;
      masks[3].shiftin   = 0;
      masks[3].max_value = (float)0.0;
      break;

    case 16:
      masks[0].mask      = 0x7c00;
      masks[0].shiftin   = 10;
      masks[0].max_value = (float)31.0;
      masks[1].mask      = 0x03e0;
      masks[1].shiftin   = 5;
      masks[1].max_value = (float)31.0;
      masks[2].mask      = 0x001f;
      masks[2].shiftin   = 0;
      masks[2].max_value = (float)31.0;
      masks[3].mask      = 0x0;
      masks[3].shiftin   = 0;
      masks[3].max_value = (float)0.0;
      break;

    default:
      break;
    }
}