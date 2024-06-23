struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color: vec3f,
}

struct FragmentInput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
}

@vertex
fn vertex_main(in: VertexInput) -> FragmentInput {
    var out: FragmentInput;
    let ratio = 640.0 / 480.0;
    let offset = vec2f(-0.6875, -0.463);
    out.position += vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
    out.color = in.color;
    return out;
}

@fragment
fn fragment_main(in: FragmentInput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
