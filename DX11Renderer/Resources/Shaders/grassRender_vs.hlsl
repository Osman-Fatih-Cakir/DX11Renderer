
cbuffer MatrixBuffer
{
  row_major matrix worldMatrix;
  row_major matrix viewMatrix;
  row_major matrix projectionMatrix;
};

struct VertexInputType
{
  float4 position : POSITION;
  float3 normal : NORMAL;
  float2 texCoord : TEXCOORD;
};

struct PixelInputType
{
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float2 texCoord : TEXCOORD;
};

PixelInputType Main(VertexInputType input, uint instanceID : SV_InstanceID)
{
  PixelInputType output;

  input.position.w = 1.0f; // TODO use vec3 for pos and the normal

  // TODO
  matrix scaleMatrix = {
    0.01f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.01f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.01f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
 
  scaleMatrix = mul(worldMatrix, scaleMatrix);

  output.position = mul(mul(mul(input.position, scaleMatrix), viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu

  output.normal = input.normal;
  output.texCoord = input.texCoord;

  return output;
}
