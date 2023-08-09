
#define GRASS_COUNT_PER_EDGE 20
#define GRASS_COUNT_PER_TILE GRASS_COUNT_PER_EDGE * GRASS_COUNT_PER_EDGE
#define MIN_TILE_MIN_WORLD_POS_X -6
#define MAX_TILE_MAX_WORLD_POS_X 6
#define MIN_TILE_MIN_WORLD_POS_Z -6
#define MAX_TILE_MAX_WORLD_POS_Z 6

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

float3 CalcWind(float4 worldPos, float4 instanceWorldPos, float4 tilePos, float height)
{
  float totalTileSizeX = MAX_TILE_MAX_WORLD_POS_X - MIN_TILE_MIN_WORLD_POS_X;
  float totalTileSizeZ = MAX_TILE_MAX_WORLD_POS_Z - MIN_TILE_MIN_WORLD_POS_Z;
  float x = (instanceWorldPos.x + tilePos.x - MIN_TILE_MIN_WORLD_POS_X) / totalTileSizeX;
  float z = (instanceWorldPos.z + tilePos.z - MIN_TILE_MIN_WORLD_POS_Z) / totalTileSizeZ;
  
  x /= 16.0f;
  z /= 16.0f;
  float2 noiseTexCoord = { x, z };
  float timeFactor = (time % 100000) / 100000.0f;
  timeFactor = timeFactor * 15.0f / 16.0f;
  noiseTexCoord.x += timeFactor;

  float3 noise = noiseTexture.SampleLevel(samplerState, noiseTexCoord, 0).xyz;
  noise *= 20.0f;
  float3 windedWorldPos = worldPos.xyz;
  windedWorldPos += noise * log(height + 1.0f) * 1.1f;

  // avoid stretching
  float3 rootPos = instanceWorldPos.xyz;
  float3 ttt = worldPos.xyz - rootPos;
  float length = sqrt(ttt.x * ttt.x + ttt.y * ttt.y + ttt.z * ttt.z); // 20 is scaling done on cpu
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

  float height = worldPos.y / (0.6f * 20.0f); // 20 is scaling done on cpu
  worldPos.xyz = CalcWind(worldPos, instanceWorldPos, tilePosition, height);
  worldPos += tilePosition;

  output.position = mul(mul(worldPos, viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu
  output.height = height;

  return output;
}
