// WaterShader.h
#ifndef _WATERSHADER_H_
#define _WATERSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
private:
    struct TimeBufferType
    {
        float time;
        float waveAmplitude;
        float waveFrequency;
        float waveSpeed;
    };

    struct LightBufferType
    {
        XMFLOAT4 ambient;
        XMFLOAT4 diffuse;
        XMFLOAT3 direction;
        float padding;
    };

public:
    WaterShader(ID3D11Device* device, HWND hwnd);
    ~WaterShader();

    void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection,
        ID3D11ShaderResourceView* texture, Light* light, float time, float amplitude = 0.5f, float frequency = 1.0f, float speed = 1.0f);

private:
    void initShader(const wchar_t* vs, const wchar_t* ps);

private:
    ID3D11Buffer* matrixBuffer;
    ID3D11SamplerState* sampleState;
    ID3D11Buffer* timeBuffer;
    ID3D11Buffer* lightBuffer;
};

#endif
