cbuffer CB_Object_Info : register(b0) {
	matrix gmtx_World : packoffset(c0);
}

cbuffer CB_Camera_Info : register(b1) {
	matrix gmtx_View : packoffset(c0);
	matrix gmtx_Projection : packoffset(c4);
}

cbuffer CB_UI_Info : register(b2) {
	float4 f4Color : packoffset(c0);
}

struct INSTANCED_OBJECT_INFO {
	matrix m_mtx_Object;
	float4 m_f4Color;
};

StructuredBuffer<INSTANCED_OBJECT_INFO> gObject_Infos : register(t0);

struct VS_INPUT {
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VSDiffused(VS_INPUT input) {
	VS_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtx_World), gmtx_View), gmtx_Projection);
	output.color = input.color;

	return output;
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET{
	return input.color;
}

//
struct VS_INSTANCING_INPUT {
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCING_OUTPUT {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID) {
	VS_INSTANCING_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gObject_Infos[nInstanceID].m_mtx_Object), gmtx_View), gmtx_Projection);
	output.color = gObject_Infos[nInstanceID].m_f4Color;

	return output;
}

VS_INSTANCING_OUTPUT VSInstancing4BoudingBox(VS_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID) {
	VS_INSTANCING_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gObject_Infos[nInstanceID].m_mtx_Object), gmtx_View), gmtx_Projection);
	output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return output;
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET{
	return input.color;
}

//
VS_OUTPUT VSUI(VS_INPUT input) {
	VS_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.color = f4Color;

	return output;
}