
/* NOTE:
* To achieve more occupancy and efficiency the texture is 36x36x16
* Thread group has 36x36x1 threads. There are 16 thread groups.
* The swizzling is efficient because when we sample from the texture we only access different x and y coordinates from the same slice of the 3D texture per thread group
* (and I think this is more efficient than accessing different texture slices in the same thread group because of the texture texture caching. I might be wrong too btw)
* When we sample the texture for using, we will treat the y axis as z and z axis as y
*/

#define WIND_TEXTURE_WIDTH 36
#define WIND_TEXTURE_HEIGHT 36
#define WIND_TEXTURE_DEPTH 16

Texture2D<float4> PrevState : register(t0);
RWTexture2D<float4> NextState : register(u0);

[numthreads(1, 1, 1)]
void Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint2 coord = { dispatchThreadID.x, dispatchThreadID.y };
	NextState[coord] = PrevState[coord];
}