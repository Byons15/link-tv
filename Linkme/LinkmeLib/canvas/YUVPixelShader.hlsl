Texture2D tx : register(t0);
SamplerState samLinear : register(s0);
RWBuffer<uint> yuvBuffer : register(u1);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 YUVPSMain(PS_INPUT frag) : SV_Target
{
    float4 fRGB = tx.Sample(samLinear, frag.tex);
    
    uint4 uRGB = fRGB * 255;
    int3 yuv;
    
    yuv[0] = (int) ((0.257 * uRGB.r) + (0.504 * uRGB.g) + (0.098 * uRGB.b) + 16);
    yuv[1] = (int) (-(0.148 * uRGB.r) - (0.291 * uRGB.g) + (0.439 * uRGB.b) + 128);
    yuv[2] = (int) ((0.439 * uRGB.r) - (0.368 * uRGB.g) - (0.071 * uRGB.b) + 128);
    
    yuv[0] = clamp(yuv[0], 0, 255);
    
    int3 offset;
    
    uint w, h;
    tx.GetDimensions(w, h);
    
    uint2 pos = uint2((uint) (frag.pos.x - 0.5f), (uint) (frag.pos.y - 0.5f));
    
    // 保存Y分量
    offset[0] = pos.y * w + pos.x;
    yuvBuffer[offset[0]] = yuv[0];
    
    // 保存U分量
    if (pos.y % 2 == 0 && pos.x % 2 == 0)
    {
        offset[1] = w * h + w / 2 * pos.y / 2 + pos.x / 2;
        yuv[1] = clamp(yuv[1], 0, 255);
        yuvBuffer[offset[1]] = yuv[1];
        
        offset[2] = w * h + w * h / 4
                  + w / 2 * pos.y / 2 + pos.x / 2;
        yuv[2] = clamp(yuv[2], 0, 255);
        yuvBuffer[offset[2]] = yuv[2];
    }
    
    return fRGB;
}