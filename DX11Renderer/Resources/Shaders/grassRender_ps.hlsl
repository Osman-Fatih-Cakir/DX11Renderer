
struct PixelInputType
{
  float4 position : SV_POSITION;
  float height : HEIGHT;
};

float4 Main(PixelInputType input) : SV_TARGET
{
  float4 color = float4(0.4f, 0.9f, 0.2f, 1.0f)* input.height;

  return color;
}
