static void FixSignedValues(PixelPacket *q, int y)
{
  while(y-->0)
  {
     /* Please note that negative values will overflow
        Q=8; QuantumRange=255: <0;127> + 127+1 = <128; 255>
           <-1;-128> + 127+1 = <0; 127> */
    SetPixelRed(q,GetPixelRed(q)+QuantumRange/2+1);
    SetPixelGreen(q,GetPixelGreen(q)+QuantumRange/2+1);
    SetPixelBlue(q,GetPixelBlue(q)+QuantumRange/2+1);
    q++;
  }
}