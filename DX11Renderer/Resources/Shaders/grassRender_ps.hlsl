
struct PixelInputType
{
  float4 position : SV_POSITION;
};

float4 Main(PixelInputType input) : SV_TARGET
{
  return float4(0.2, 1.0f, 0.2, 1.0);
}
