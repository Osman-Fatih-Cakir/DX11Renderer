
#define GRASS_COUNT_PER_EDGE 20
#define GRASS_COUNT_PER_TILE GRASS_COUNT_PER_EDGE * GRASS_COUNT_PER_EDGE
#define MIN_TILE_MIN_WORLD_POS_X -10
#define MAX_TILE_MAX_WORLD_POS_X 26
#define MIN_TILE_MIN_WORLD_POS_Z -14
#define MAX_TILE_MAX_WORLD_POS_Z 14
#define RADIUS 36.0f

Texture2D noiseTexture : register(t0);
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

float3 SampleNoiseTexture(float4 instanceWorldPos, float4 tilePos, float speed)
{
  float totalTileSizeX = MAX_TILE_MAX_WORLD_POS_X - MIN_TILE_MIN_WORLD_POS_X;
  float totalTileSizeZ = MAX_TILE_MAX_WORLD_POS_Z - MIN_TILE_MIN_WORLD_POS_Z;
  float x = (instanceWorldPos.x + tilePos.x - MIN_TILE_MIN_WORLD_POS_X) / totalTileSizeX;
  float z = (instanceWorldPos.z + tilePos.z - MIN_TILE_MIN_WORLD_POS_Z) / totalTileSizeZ;

  x /= 8.0f;
  z /= 8.0f;
  float2 noiseTexCoord = { x, z };
  int rem = (int)(100000.0f / speed);
  float remf = (float)rem;
  float timeFactor = (time % rem) / remf;
  timeFactor = timeFactor * 7.0f / 8.0f;
  noiseTexCoord.x += timeFactor;

  float3 noise = noiseTexture.SampleLevel(samplerState, noiseTexCoord, 0).xyz;

  return noise;
}

float3 RepairStretch(float3 instanceWorldPos, float3 worldPos, float3 windedWorldPos)
{
  float3 rootPos = instanceWorldPos;
  float3 ttt = worldPos - rootPos;
  float length = sqrt(ttt.x * ttt.x + ttt.y * ttt.y + ttt.z * ttt.z);
  float3 currentDirection = normalize(windedWorldPos - rootPos);
  windedWorldPos = rootPos + currentDirection * length;
  return windedWorldPos;
}

float3 CalcDirectionalWind(float4 worldPos, float4 instanceWorldPos, float4 tilePos, float height, float distanceFade)
{
  float3 noise = SampleNoiseTexture(instanceWorldPos, tilePos, 1.5f);
  
  noise *= 0.8f;
  float3 windedWorldPos = worldPos.xyz;
  windedWorldPos += noise * height * height * distanceFade;

  // avoid stretching
  windedWorldPos = RepairStretch(instanceWorldPos.xyz, worldPos.xyz, windedWorldPos);

  return windedWorldPos;
}

float3 CalcOmniWind(float4 worldPos, float4 instanceWorldPos, float4 tilePos, float height, float2 mouseXZ, float distanceFade)
{
  float3 noise = SampleNoiseTexture(instanceWorldPos, tilePos, 1.0f);

  noise *= 0.4f;
  float3 windedWorldPos = worldPos.xyz;

  float3 dir = { mouseXZ.x - (worldPos.x + tilePos.x), 1.0f, mouseXZ.y - (worldPos.z + tilePos.z) };
  dir = normalize(dir);
  dir *= -abs(noise);

  windedWorldPos += dir * height * height * distanceFade;

  // avoid stretching
  windedWorldPos = RepairStretch(instanceWorldPos.xyz, worldPos.xyz, windedWorldPos);

  return windedWorldPos;
}

PixelInputType Main(VertexInputType input, uint instanceID : SV_InstanceID)
{
  PixelInputType output;
 
  float4x4 worldMatrix = instanceWorldMatrices[instanceID];
  float4 worldPos = mul(input.position, worldMatrix);
  float4 instanceWorldPos = worldMatrix[3];

  float height = worldPos.y / 0.6f;

  // radius check
  float2 rad = { instanceWorldPos.x + tilePosition.x - mouseXZ.x, instanceWorldPos.z + tilePosition.z - mouseXZ.y };
  float dist = sqrt(rad.x * rad.x + rad.y * rad.y);
  if (dist < RADIUS)
  {
    float distanceFade = (RADIUS - dist) / RADIUS;
    if (windType == 0) // directional
    {
      worldPos.xyz = CalcDirectionalWind(worldPos, instanceWorldPos, tilePosition, height, distanceFade);
    }
    else // if (windType == 1)
    {
      worldPos.xyz = CalcOmniWind(worldPos, instanceWorldPos, tilePosition, height, mouseXZ, distanceFade);
    }
  }
  worldPos += tilePosition;

  output.position = mul(mul(worldPos, viewMatrix), projectionMatrix); // TODO create a mvp matrix on cpu
  output.height = height;

  return output;
}
