
cbuffer MatrixBuffer
{
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};

struct VertexInputType
{
  float4 position : POSITION;
  float2 texCoord : TEXCOORD0;
};

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 texCoord : TEXCOORD0;
};

PixelInputType Main(VertexInputType input)
{
  PixelInputType output;

  input.position.w = 1.0f;

  output.position = mul(mul(mul(input.position, worldMatrix), viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu

  output.texCoord = input.texCoord;

  return output;
}