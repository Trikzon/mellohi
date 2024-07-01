struct Uniforms {
    projection: mat4x4f,
    view: mat4x4f,
    model: mat4x4f,
}

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) color: vec3f,
}

struct FragmentInput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
}

@group(0) @binding(0) var<uniform> uUniforms: Uniforms;

@vertex
fn vertex_main(in: VertexInput) -> FragmentInput {
    var out: FragmentInput;
    out.position = uUniforms.projection * uUniforms.view * uUniforms.model * vec4f(in.position, 1.0);
    out.color = in.color;
    return out;
}

@fragment
fn fragment_main(in: FragmentInput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
