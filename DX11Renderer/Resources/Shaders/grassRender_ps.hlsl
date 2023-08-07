
Texture2D colorTexture : register(t0);
SamplerState samplerState : register(s0);

struct PixelInputType
{
  float4 position : SV_POSITION;
  float4 normal : NORMAL;
  float2 texCoord : TEXCOORD;
};

float4 Main(PixelInputType input) : SV_TARGET
{
  float4 textureColor = colorTexture.Sample(samplerState, input.texCoord);

  if (textureColor.a < 0.1f)
  {
    discard;
  }

  return textureColor;
}
