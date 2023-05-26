
Texture2D colorTexture : register(t0);
SamplerState samplerState : register(s0);

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD0;
};

float4 Main(PixelInputType input) : SV_TARGET
{
  float textureColor = colorTexture.Sample(samplerState, input.texCoord);
  return textureColor;
}
