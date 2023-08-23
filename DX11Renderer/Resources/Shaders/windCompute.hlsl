
/* NOTE:
* To achieve more occupancy and efficiency the texture is 64x64x16
* Thread group has 16x16x1 threads. There are 4x4x16 thread groups.
* The swizzling is efficient because when we sample from the texture we only access different x and y coordinates from the same slice of the 3D texture per thread group
* (and I think this is more efficient than accessing different texture slices in the same thread group because of the texture caching. I might be wrong too btw)
* When we sample the texture for using, we will treat the y axis as z and z axis as y
*/

#define WIND_TEXTURE_WIDTH 64
#define WIND_TEXTURE_HEIGHT 64
#define WIND_TEXTURE_DEPTH 16

#define NOISE_TEXTURE_SIZE 256

#define RADIUS 4.0f
#define RADIUS_SQUARE (RADIUS * RADIUS)

#define PI 3.14159265f

Texture2D<float4> NoiseTexture : register(t0);
Texture3D<float4> Prev_WindFlowMap : register(t1);
RWTexture3D<float4> Next_WindFlowMap : register(u0);

cbuffer CBuffer : register(b0)
{
  float2 mouseXZ;
  float2 deltaMouseXZ;
  uint time; // total time in milliseconds * 10
  uint deltaTime; // delta time in microseconds
  uint windType; // 0: directional, 1: omni, 2: circular, 3: movement wind
}

float3 SampleNoiseTexture(uint2 coord, float speed)
{
  uint timeFactor = time * speed;
  uint2 noiseTexCoord = uint2((coord.x + timeFactor) % NOISE_TEXTURE_SIZE, (coord.y + timeFactor) % NOISE_TEXTURE_SIZE);
  float3 noise = NoiseTexture[noiseTexCoord].xyz;

  return noise;
}

float3 CalcDirectionalWind(uint2 coord, float3 windDir)
{
  float3 noise = SampleNoiseTexture(coord, 0.0025f);
  noise *= 0.6f;
  float3 wind = normalize(windDir) * noise;

  return wind;
}

float3 CalcOmniWind(uint2 coord, int2 worldCoord, float2 mouseXZ)
{
  float3 noise = SampleNoiseTexture(coord, 0.0025f);
  noise *= 0.6f;
  float3 dir = { mouseXZ.x - worldCoord.x, 0.0f, mouseXZ.y - worldCoord.y };
  dir = normalize(dir);
  dir *= -abs(noise);

  float3 wind = dir;

  return wind;
}

float3 CalcMovementWind(uint2 coord, int2 worldCoord, float2 mouseXZ, float3 windDir)
{
  float3 noise = SampleNoiseTexture(coord, 0.0025f);
  noise *= 0.6f;

  float3 dir = { worldCoord.x - mouseXZ.x, windDir.y, worldCoord.y - mouseXZ.y };
  dir = normalize(dir);
  windDir = normalize(windDir) * 2.0f;
  dir += windDir;

  float3 wind = normalize(dir) * noise;

  return wind;
}

float CalcMovementWindMultiplier(float lastVal, float fadeSpeed)
{
  return clamp(lastVal - fadeSpeed, 0.0f, 1.0f);
}

[numthreads(16, 16, 1)]
void Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
  uint3 coord = dispatchThreadID;
  float4 wind = Prev_WindFlowMap[coord];

  // the 64x64x16 sized wind texture represents the wind simulation for the coordinates {x:(-32,+32), y:(-32,+32), z:(0,+16)}
  int2 worldCoord = { dispatchThreadID.x - (WIND_TEXTURE_WIDTH / 2), dispatchThreadID.y - (WIND_TEXTURE_HEIGHT / 2) };
  
  float2 rad = { (float)worldCoord.x - mouseXZ.x, (float)worldCoord.y - mouseXZ.y };
  float distSquare = rad.x * rad.x + rad.y * rad.y;
  bool insideRadius = distSquare < RADIUS_SQUARE;

  if (windType == 0 && insideRadius) // directional
  {
    float3 windDir = { 1.0f, 1.0f, 1.0f };
    wind.xyz = CalcDirectionalWind(coord.xy, windDir);

    float distanceFade = 1.0f - (distSquare / RADIUS_SQUARE);
    wind.xyz *= distanceFade;
  }
  else if (windType == 1 && insideRadius) // omni-directional
  {
    wind.xyz = CalcOmniWind(coord.xy, worldCoord, mouseXZ);

    float distanceFade = 1.0f - (distSquare / RADIUS_SQUARE);
    wind.xyz *= distanceFade;
  }
  else if (windType == 2) // circular
  {
    //wind.xyz = CalcCircularWind(); // TODO
  }
  else // if windType == 3 // movement wind
  {
    // movement wind fade calculation
    float fadeSpeed = deltaTime * 0.00000002f;
    wind.w = CalcMovementWindMultiplier(wind.w, fadeSpeed);
    float3 lastWindDir = Next_WindFlowMap[coord].xyz; // sample 2 previous frame data
    float3 windDir = lastWindDir;

    // wind direction
    if (insideRadius)
    {
      float3 newWindDir = float3(deltaMouseXZ.x, 0.0f, deltaMouseXZ.y);

      float2 mouseToWorld = normalize(float2(worldCoord.x - mouseXZ.x, worldCoord.y - mouseXZ.y));
      bool inFront = dot(normalize(newWindDir.xz), mouseToWorld) > cos(PI / 4.0f);
      if (inFront)
      {
        windDir = newWindDir;
        wind.w = 1.0f;

        float distanceFade = 1.0f - (distSquare / RADIUS_SQUARE);
        wind.xyz = CalcMovementWind(coord.xy, worldCoord, mouseXZ, windDir) * distanceFade;
      }
    }

    wind.xyz = (wind.xyz + lastWindDir) / 2.0f;

    // apply movement wind fade
    wind.xyz *= wind.w;
  }

  Next_WindFlowMap[coord] = wind;
}
