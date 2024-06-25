struct MyUniforms {
    time: f32,
    color: vec4f,
}

struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color: vec3f,
}

struct FragmentInput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
}

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

@vertex
fn vertex_main(in: VertexInput) -> FragmentInput {
    var out: FragmentInput;
    let ratio = 640.0 / 480.0;
    var offset = vec2f(-0.6875, -0.463);
    offset += 0.3 * vec2f(cos(uMyUniforms.time), sin(uMyUniforms.time));
    out.position += vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
    out.color = in.color + uMyUniforms.color.rgb;
    return out;
}

@fragment
fn fragment_main(in: FragmentInput) -> @location(0) vec4f {
    return vec4f(in.color, uMyUniforms.color.a);
}
