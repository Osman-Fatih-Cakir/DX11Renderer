
cbuffer MatrixBuffer
{
  row_major matrix worldMatrix;
  row_major matrix viewMatrix;
  row_major matrix projectionMatrix;
};

struct VertexInputType
{
  float4 position : POSITION;
  float4 color : COLOR;
};

struct PixelInputType
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

PixelInputType Main(VertexInputType input)
{
  PixelInputType output;

  input.position.w = 1.0f;

  output.position = mul(mul(mul(input.position, worldMatrix), viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu

  output.color = input.color;

  return output;
}
