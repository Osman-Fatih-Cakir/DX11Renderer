
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

#define RADIUS 2.0f
#define RADIUS_SQUARE (RADIUS * RADIUS)

Texture2D<float4> NoiseTexture : register(t0);
Texture2D<float4> Prev_WindFlowMap : register(t1);
RWTexture3D<float4> Next_WindFlowMap : register(u0);

cbuffer CBuffer : register(b0)
{
  float2 mouseXZ;
  uint time;
  uint windType; // 0: directional, 1: omni, 2: circular
}

float3 SampleNoiseTexture(uint2 coord, float speed)
{
  uint timeFactor = time * speed;
  uint2 noiseTexCoord = uint2((coord.x + timeFactor) % NOISE_TEXTURE_SIZE, (coord.y + timeFactor) % NOISE_TEXTURE_SIZE);
  float3 noise = NoiseTexture[noiseTexCoord];

  return noise;
}

float3 CalcDirectionalWind(uint2 coord, float distanceFade)
{
  float3 noise = SampleNoiseTexture(coord, 0.0025f);
  noise *= 0.8f;
  float3 wind = noise * distanceFade;

  return wind;
}

float3 CalcOmniWind(uint2 coord, int2 worldCoord, float2 mouseXZ, float distanceFade)
{
  float3 noise = SampleNoiseTexture(coord, 0.0025f);
  noise *= 0.4f;
  float3 dir = { mouseXZ.x - worldCoord.x, 0.0f, mouseXZ.y - worldCoord.y };
  dir = normalize(dir);
  dir *= -abs(noise);

  float3 wind = dir * distanceFade;

  return wind;
}

float CalcMovementWindMultiplier(float lastVal, float fadeSpeed)
{
  //todo

  return 1.0f;
}

[numthreads(16, 16, 1)]
void Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
  // moving wind fade calculation

  //todo float fadeSpeed = 1.0f;
  //todo wind.w = CalcMovementWindMultiplier(wind.w, fadeSpeed);

  // wind transformation calculation

  float4 wind = { 0.0f, 0.0f, 0.0f, 0.0f };
  uint3 coord = dispatchThreadID;

  // the 64x64x16 sized wind texture represents the wind simulation for the coordinates {x:(-32,+32), y:(-32,+32), z:(0,+16)}
  int2 worldCoord = { dispatchThreadID.x - (WIND_TEXTURE_WIDTH / 2), dispatchThreadID.y - (WIND_TEXTURE_HEIGHT / 2) };
  
  float2 rad = { (float)worldCoord.x - mouseXZ.x, (float)worldCoord.y - mouseXZ.y };
  float distSquare = rad.x * rad.x + rad.y * rad.y;
  if (distSquare < RADIUS_SQUARE)
  {
    float distanceFade = 1.0f - (distSquare / RADIUS_SQUARE);

    if (windType == 0) // directional
    {
      wind.xyz = CalcDirectionalWind(coord.xy, distanceFade);
    }
    else if (windType == 1) // omni-directional
    {
      wind.xyz = CalcOmniWind(coord.xy, worldCoord, mouseXZ, distanceFade);
    }
    else // if windType == 2 // circular
    {
      //wind.xyz = CalcCircularWind(); // TODO
    }

    //todo wind.w = 1.0f;
  }

  // apply movement wind fade
  //todo wind.xyz *= wind.w;

  Next_WindFlowMap[coord] = wind;
}
