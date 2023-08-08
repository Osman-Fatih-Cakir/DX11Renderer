
#define GRASS_COUNT 441

cbuffer PerFrameBuffer : register(b0)
{
  row_major float4x4 viewMatrix;
  row_major float4x4 projectionMatrix;

  // another constant buffer called PerTileBuffer can be used for below
  // but this will to do the trick for now
  float4 tilePosition;
};

cbuffer PerSceneBuffer : register(b1)
{
  row_major float4x4 instanceWorldMatrices[GRASS_COUNT];
}

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
 
  float4x4 worldMatrix = instanceWorldMatrices[instanceID];
  float4 worldPos = mul(input.position, worldMatrix);
  worldPos += tilePosition;

  output.position = mul(mul(worldPos, viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu

  output.normal = input.normal;
  output.texCoord = input.texCoord;

  return output;
}
