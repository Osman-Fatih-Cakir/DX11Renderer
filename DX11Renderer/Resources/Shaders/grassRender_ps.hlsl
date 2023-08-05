
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
  float2 revertedTexCoord = input.texCoord;
  revertedTexCoord.y = 1.0f - revertedTexCoord.y; // TODO reverse invert
  float4 textureColor = colorTexture.Sample(samplerState, revertedTexCoord);
  return textureColor;
}
