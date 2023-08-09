
#define GRASS_COUNT_PER_EDGE 20
#define GRASS_COUNT_PER_TILE GRASS_COUNT_PER_EDGE * GRASS_COUNT_PER_EDGE
#define TILE_MIN_WORLD_POS_X -2
#define TILE_MAX_WORLD_POS_X 2
#define TILE_MIN_WORLD_POS_Z -2
#define TILE_MAX_WORLD_POS_Z 2

Texture2D noiseTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer PerFrameBuffer : register(b0)
{
  row_major float4x4 viewMatrix;
  row_major float4x4 projectionMatrix;

  // another constant buffer called PerTileBuffer can be used for below
  // but this will to do the trick for now
  float4 tilePosition;
  uint2 tileCoord;
  uint time;
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

float3 CalcWind(float4 worldPos, uint instanceID, float height)
{
  float tileSizeX = TILE_MAX_WORLD_POS_X - TILE_MIN_WORLD_POS_X;
  float tileSizeZ = TILE_MAX_WORLD_POS_Z - TILE_MIN_WORLD_POS_Z;
  float x = (worldPos.x - TILE_MIN_WORLD_POS_X) / tileSizeX;
  float z = (worldPos.z - TILE_MIN_WORLD_POS_Z) / tileSizeZ;
  float2 noiseTexCoord = { x, z };
  float timeFactor = (time % 150000) / 150000.0f;
  noiseTexCoord += timeFactor;
  noiseTexCoord %= 1.0f;

  float3 noise = noiseTexture.SampleLevel(samplerState, noiseTexCoord, 0).xyz;
  noise *= 20.0f;
  float3 windedWorldPos = worldPos.xyz;
  windedWorldPos += noise * log(height + 1.0f) * 1.1f;

  // avoid stretching
  // I dont have access to the root of the grasses, so it is just an approximation
  float length = worldPos.y;
  float3 rootPos = { worldPos.x, 0.0f, worldPos.z };
  float3 currentDirection = normalize(windedWorldPos - rootPos);
  windedWorldPos = rootPos + currentDirection * length * 1.1f;

  return windedWorldPos;
}

PixelInputType Main(VertexInputType input, uint instanceID : SV_InstanceID)
{
  PixelInputType output;
 
  float4x4 worldMatrix = instanceWorldMatrices[instanceID];
  float4 worldPos = mul(input.position, worldMatrix);

  float height = worldPos.y / (0.6f * 30.0f); // 20 is scaling done on cpu
  worldPos.xyz = CalcWind(worldPos, instanceID, height);
  worldPos += tilePosition;

  output.position = mul(mul(worldPos, viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu
  output.height = height;

  return output;
}
