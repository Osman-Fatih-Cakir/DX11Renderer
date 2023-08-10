
struct PixelInputType
{
  float4 position : SV_POSITION;
  float height : HEIGHT;
};

float4 Main(PixelInputType input) : SV_TARGET
{
  float4 color = float4(0.0f, 0.35f, 0.09f, 1.0f) * input.height * 0.7f;

  return color;
}
