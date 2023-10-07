shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back, diffuse_burley, specular_schlick_ggx;
uniform sampler2D texture_albedo : source_color, filter_nearest_mipmap, repeat_enable;

void fragment() {
	ALBEDO = texture(texture_albedo, UV).rgb;
}
