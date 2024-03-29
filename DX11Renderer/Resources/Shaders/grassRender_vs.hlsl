
#define GRASS_COUNT_PER_TILE 20

#define WIND_TEXTURE_WIDTH 64
#define WIND_TEXTURE_HEIGHT 64
#define WIND_TEXTURE_DEPTH 16

Texture3D windTexture : register(t0);
SamplerState samplerState : register(s0);

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
  row_major float4x4 instanceWorldMatrices[GRASS_COUNT_PER_TILE];
}

struct VertexInputType
{
  float4 position : POSITION;
};

struct PixelInputType
{
  float4 position : SV_POSITION;
  float height : HEIGHT;
};

float3 RepairStretch(float3 instanceWorldPos, float3 worldPos, float3 windedWorldPos)
{
  float3 rootPos = instanceWorldPos;
  float3 ttt = worldPos - rootPos;
  float length = sqrt(ttt.x * ttt.x + ttt.y * ttt.y + ttt.z * ttt.z);
  float3 currentDirection = normalize(windedWorldPos - rootPos);
  windedWorldPos = rootPos + currentDirection * length;
  return windedWorldPos;
}

PixelInputType Main(VertexInputType input, uint instanceID : SV_InstanceID)
{
  PixelInputType output;

  float4x4 worldMatrix = instanceWorldMatrices[instanceID];
  float4 worldPos = mul(input.position, worldMatrix);
  float4 instanceWorldPos = worldMatrix[3];

  // max vertex y is 0.06 for this grass blade object
  float height = worldPos.y / 1.2f;

  float x = (0.5f + instanceWorldPos.x + tilePosition.x + (WIND_TEXTURE_WIDTH / 2)) / WIND_TEXTURE_WIDTH;
  float z = (0.5f + instanceWorldPos.z + tilePosition.z + (WIND_TEXTURE_HEIGHT / 2)) / WIND_TEXTURE_HEIGHT;
  float3 coord = { x, z, 0.0f };
  float3 wind = windTexture.SampleLevel(samplerState, coord, 0).xyz * height * height;
  float4 windedWorldPos = worldPos;
  windedWorldPos.xyz += wind;
  windedWorldPos.xyz = RepairStretch(instanceWorldPos.xyz, worldPos.xyz, windedWorldPos.xyz);
  windedWorldPos.xyz += tilePosition;

  output.position = mul(mul(windedWorldPos, viewMatrix), projectionMatrix);
  output.height = height;

  return output;
}
