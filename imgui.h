#ifndef JFG_IMGUI_H
#define JFG_IMGUI_H

#include "prelude.h"
#include "imgui_gpu_data_types.h"
// XXX - needed for memset, should remove this later
#include <string.h>

#ifdef JFG_D3D11_H

#ifndef IMGUI_DEFINE_GFX
#define IMGUI_DEFINE_GFX
#endif

struct IMGUI_D3D11_Context
{
	ID3D11Texture2D          *text_texture;
	ID3D11ShaderResourceView *text_texture_srv;
	ID3D11VertexShader       *text_vertex_shader;
	ID3D11PixelShader        *text_pixel_shader;
	ID3D11Buffer             *text_instance_buffer;
	ID3D11ShaderResourceView *text_instance_buffer_srv;
	ID3D11Buffer             *text_constant_buffer;
	ID3D11RasterizerState    *text_rasterizer_state;
};

#endif

#define IMGUI_MAX_TEXT_CHARACTERS 4096

struct IMGUI_Context
{
	v2 text_pos;
	v4 text_color;
	u32 text_index;
	IMGUI_VS_Text_Instance text_buffer[IMGUI_MAX_TEXT_CHARACTERS];
#ifdef IMGUI_DEFINE_GFX
	union {
	#ifdef JFG_D3D11_H
		IMGUI_D3D11_Context d3d11;
	#endif
	};
#endif
};

void imgui_begin(IMGUI_Context* context);
void imgui_set_text_cursor(IMGUI_Context* context, v4 color, v2 pos);
void imgui_text(IMGUI_Context* context, char* text);

#ifndef JFG_HEADER_ONLY
void imgui_begin(IMGUI_Context* context)
{
	context->text_pos = { 0.0f, 0.0f };
	context->text_color = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->text_index = 0;
}

void imgui_set_text_cursor(IMGUI_Context* context, v4 color, v2 pos)
{
	context->text_pos = pos;
	context->text_color = color;
}

void imgui_text(IMGUI_Context* context, char* text)
{
	v2 pos = context->text_pos;
	v4 color = context->text_color;
	u32 index = context->text_index;
	IMGUI_VS_Text_Instance *cur_char = &context->text_buffer[index];
	for (u8 *p = (u8*)text; *p; ++p) {
		if (index == IMGUI_MAX_TEXT_CHARACTERS) {
			break;
		}
		u8 c = *p;
		switch (c) {
		case '\n':
			pos.x = 0.0f;
			++pos.y;
			break;
		case '\t':
			// XXX - ignore proper tab behaviour
			pos.x += 8.0f;
			break;
		case ' ':
			++pos.x;
			break;
		default:
			cur_char->glyph = c;
			cur_char->pos = pos;
			cur_char->color = color;
			++index;
			++cur_char;
			++pos.x;
			break;
		}
	}
	ASSERT(index <= IMGUI_MAX_TEXT_CHARACTERS);
	context->text_index = index;
	context->text_pos.y += pos.y - context->text_pos.y + 1;
}
#endif

// d3d11 implementation
#ifdef JFG_D3D11_H

u8 imgui_d3d11_init(IMGUI_Context* context, ID3D11Device* device);
void imgui_d3d11_free(IMGUI_Context* context);
void imgui_d3d11_draw(IMGUI_Context*          imgui,
                      ID3D11DeviceContext*    dc,
                      ID3D11RenderTargetView* output_rtv,
                      v2_u32                  output_view_dimensions);

#ifndef JFG_HEADER_ONLY
#include "imgui_dxbc_text_vertex_shader.data.h"
#include "imgui_dxbc_text_pixel_shader.data.h"
#include "codepage_437.h"

u8 imgui_d3d11_init(IMGUI_Context* context, ID3D11Device* device)
{
	HRESULT hr;

	ID3D11Texture2D *text_texture;
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = TEXTURE_CODEPAGE_437.width;
		desc.Height = TEXTURE_CODEPAGE_437.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data_desc = {};
		data_desc.pSysMem = TEXTURE_CODEPAGE_437.data;
		data_desc.SysMemPitch =
			TEXTURE_CODEPAGE_437.width * sizeof(TEXTURE_CODEPAGE_437.data[0]);
		data_desc.SysMemSlicePitch = 0;

		hr = device->CreateTexture2D(&desc, &data_desc, &text_texture);
	}
	if (FAILED(hr)) {
		goto error_init_text_texture;
	}

	ID3D11ShaderResourceView *text_texture_srv;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC text_texture_srv_desc = {};
		text_texture_srv_desc.Format = DXGI_FORMAT_R8_UNORM;
		text_texture_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		text_texture_srv_desc.Texture2D.MostDetailedMip = 0;
		text_texture_srv_desc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(text_texture,
			&text_texture_srv_desc, &text_texture_srv);
	}
	if (FAILED(hr)) {
		goto error_init_text_texture_srv;
	}

	ID3D11VertexShader *text_vertex_shader;
	hr = device->CreateVertexShader(IMGUI_TEXT_VS, ARRAY_SIZE(IMGUI_TEXT_VS), NULL,
		&text_vertex_shader);
	if (FAILED(hr)) {
		goto error_init_text_vertex_shader;
	}

	ID3D11PixelShader *text_pixel_shader;
	hr = device->CreatePixelShader(IMGUI_TEXT_PS, ARRAY_SIZE(IMGUI_TEXT_PS), NULL,
		&text_pixel_shader);
	if (FAILED(hr)) {
		goto error_init_text_pixel_shader;
	}

	ID3D11Buffer *text_instance_buffer;
	{
		D3D11_BUFFER_DESC text_instance_buffer_desc = {};
		text_instance_buffer_desc.ByteWidth =
			sizeof(IMGUI_VS_Text_Instance) * IMGUI_MAX_TEXT_CHARACTERS;
		text_instance_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		text_instance_buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		text_instance_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		text_instance_buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		text_instance_buffer_desc.StructureByteStride = sizeof(IMGUI_VS_Text_Instance);

		hr = device->CreateBuffer(&text_instance_buffer_desc, NULL, &text_instance_buffer);
	}
	if (FAILED(hr)) {
		goto error_init_text_instance_buffer;
	}

	ID3D11ShaderResourceView *text_instance_buffer_srv;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC text_instance_buffer_srv_desc = {};
		text_instance_buffer_srv_desc.Format = DXGI_FORMAT_UNKNOWN;
		text_instance_buffer_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		text_instance_buffer_srv_desc.Buffer.ElementOffset = 0;
		text_instance_buffer_srv_desc.Buffer.ElementWidth = IMGUI_MAX_TEXT_CHARACTERS;

		hr = device->CreateShaderResourceView(text_instance_buffer,
			&text_instance_buffer_srv_desc, &text_instance_buffer_srv);
	}
	if (FAILED(hr)) {
		goto error_init_text_instance_buffer_srv;
	}

	ID3D11Buffer *text_constant_buffer;
	{
		D3D11_BUFFER_DESC text_constant_buffer_desc = {};
		text_constant_buffer_desc.ByteWidth = sizeof(IMGUI_CB_Text);
		text_constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		text_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		text_constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		text_constant_buffer_desc.MiscFlags = 0;
		text_constant_buffer_desc.StructureByteStride = sizeof(IMGUI_CB_Text);

		hr = device->CreateBuffer(&text_constant_buffer_desc, NULL, &text_constant_buffer);
	}
	if (FAILED(hr)) {
		goto error_init_text_constant_buffer;
	}

	ID3D11RasterizerState *text_rasterizer_state;
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;

		hr = device->CreateRasterizerState(&rasterizer_desc, &text_rasterizer_state);
	}
	if (FAILED(hr)) {
		goto error_init_text_rasterizer_state;
	}

	context->d3d11.text_texture             = text_texture;
	context->d3d11.text_texture_srv         = text_texture_srv;
	context->d3d11.text_vertex_shader       = text_vertex_shader;
	context->d3d11.text_pixel_shader        = text_pixel_shader;
	context->d3d11.text_instance_buffer     = text_instance_buffer;
	context->d3d11.text_instance_buffer_srv = text_instance_buffer_srv;
	context->d3d11.text_constant_buffer     = text_constant_buffer;
	context->d3d11.text_rasterizer_state    = text_rasterizer_state;

	return 1;

	text_rasterizer_state->Release();
error_init_text_rasterizer_state:
	text_constant_buffer->Release();
error_init_text_constant_buffer:
	text_instance_buffer_srv->Release();
error_init_text_instance_buffer_srv:
	text_instance_buffer->Release();
error_init_text_instance_buffer:
	text_pixel_shader->Release();
error_init_text_pixel_shader:
	text_vertex_shader->Release();
error_init_text_vertex_shader:
	text_texture_srv->Release();
error_init_text_texture_srv:
	text_texture->Release();
error_init_text_texture:
	return 0;
}

void imgui_d3d11_free(IMGUI_Context* context)
{
	context->d3d11.text_rasterizer_state->Release();
	context->d3d11.text_constant_buffer->Release();
	context->d3d11.text_instance_buffer_srv->Release();
	context->d3d11.text_instance_buffer->Release();
	context->d3d11.text_pixel_shader->Release();
	context->d3d11.text_vertex_shader->Release();
	context->d3d11.text_texture_srv->Release();
	context->d3d11.text_texture->Release();
}

void imgui_d3d11_draw(
	IMGUI_Context*          imgui,
	ID3D11DeviceContext*    dc,
	ID3D11RenderTargetView* output_rtv,
	v2_u32                  output_view_dimensions)
{
	IMGUI_CB_Text text_constant_buffer = {};
	text_constant_buffer.screen_size.w = (f32)output_view_dimensions.w;
	text_constant_buffer.screen_size.h = (f32)output_view_dimensions.h;
	text_constant_buffer.glyph_size.w = (f32)TEXTURE_CODEPAGE_437.glyph_width;
	text_constant_buffer.glyph_size.h = (f32)TEXTURE_CODEPAGE_437.glyph_height;
	text_constant_buffer.tex_size.w = (f32)TEXTURE_CODEPAGE_437.width;
	text_constant_buffer.tex_size.h = (f32)TEXTURE_CODEPAGE_437.height;
	text_constant_buffer.zoom = 2.0f;

	D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};

	HRESULT hr;
	hr = dc->Map(imgui->d3d11.text_instance_buffer,
		0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	ASSERT(SUCCEEDED(hr));
	memcpy(mapped_buffer.pData, &imgui->text_buffer,
		sizeof(IMGUI_VS_Text_Instance) * imgui->text_index);
	dc->Unmap(imgui->d3d11.text_instance_buffer, 0);

	hr = dc->Map(imgui->d3d11.text_constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mapped_buffer);
	ASSERT(SUCCEEDED(hr));
	memcpy(mapped_buffer.pData, &text_constant_buffer, sizeof(text_constant_buffer));
	dc->Unmap(imgui->d3d11.text_constant_buffer, 0);

	dc->IASetInputLayout(NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dc->VSSetConstantBuffers(0, 1, &imgui->d3d11.text_constant_buffer);
	dc->VSSetShaderResources(0, 1, &imgui->d3d11.text_instance_buffer_srv);
	dc->VSSetShader(imgui->d3d11.text_vertex_shader, NULL, 0);
	dc->PSSetConstantBuffers(0, 1, &imgui->d3d11.text_constant_buffer);
	dc->PSSetShaderResources(0, 1, &imgui->d3d11.text_texture_srv);
	dc->PSSetShader(imgui->d3d11.text_pixel_shader, NULL, 0);

	dc->RSSetState(imgui->d3d11.text_rasterizer_state);
	dc->OMSetRenderTargets(1, &output_rtv, NULL);

	dc->DrawInstanced(6, imgui->text_index, 0, 0);
}
#endif

#endif

#endif
