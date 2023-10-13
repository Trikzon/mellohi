shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back, diffuse_burley, specular_schlick_ggx, unshaded;

uniform sampler2D texture_albedo : source_color, filter_nearest_mipmap, repeat_enable;

void fragment() {
    float light = max(COLOR.r, COLOR.g);
	ALBEDO = texture(texture_albedo, UV).rgb * light;
}
