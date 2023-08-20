
#define GRASS_COUNT_PER_EDGE 20
#define GRASS_COUNT_PER_TILE GRASS_COUNT_PER_EDGE * GRASS_COUNT_PER_EDGE
#define MIN_TILE_MIN_WORLD_POS_X -10
#define MAX_TILE_MAX_WORLD_POS_X 26
#define MIN_TILE_MIN_WORLD_POS_Z -14
#define MAX_TILE_MAX_WORLD_POS_Z 14
#define RADIUS 2.0f

#define WIND_TEXTURE_WIDTH 64
#define WIND_TEXTURE_HEIGHT 64
#define WIND_TEXTURE_DEPTH 16

Texture2D noiseTexture : register(t0);
Texture2D windTexture : register(t1);
SamplerState samplerState : register(s0);

cbuffer PerFrameBuffer : register(b0)
{
  row_major float4x4 viewMatrix;
  row_major float4x4 projectionMatrix;

  // another constant buffer called PerTileBuffer can be used for below
  // but this will to do the trick for now
  float4 tilePosition;
  float2 mouseXZ;
  uint2 tileCoord;
  uint time;
  uint windType; // 0: directional, 1: omni
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

  float height = worldPos.y / 0.6f;

  float x = (instanceWorldPos.x + tilePosition.x + (WIND_TEXTURE_WIDTH / 2)) / WIND_TEXTURE_WIDTH;
  float z = (instanceWorldPos.z + tilePosition.z + (WIND_TEXTURE_HEIGHT / 2)) / WIND_TEXTURE_HEIGHT;
  float2 coord = { x,z };
  float3 wind = windTexture.SampleLevel(samplerState, coord, 0).xyz * height * height;
  float4 windedWorldPos = worldPos;
  windedWorldPos.xyz += wind;
  windedWorldPos.xyz = RepairStretch(instanceWorldPos.xyz, worldPos.xyz, windedWorldPos.xyz);
  windedWorldPos.xyz += tilePosition;

  output.position = mul(mul(windedWorldPos, viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu
  output.height = height;

  return output;
}
