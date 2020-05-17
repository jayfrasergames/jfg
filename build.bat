@echo off

pushd %~dp0

fxc /Fh imgui_dxbc_text_vertex_shader.data.h /E vs_text /Vn IMGUI_TEXT_VS /T vs_5_0 imgui.hlsl
fxc /Fh imgui_dxbc_text_pixel_shader.data.h  /E ps_text /Vn IMGUI_TEXT_PS /T ps_5_0 imgui.hlsl

popd
