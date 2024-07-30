struct VertexInput
{
    @location(0) position: vec2f,
    @location(1) color: vec3f,
}

struct VertexOutput
{
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
}

struct Uniforms
{
    color: vec3f,
}

@group(0) @binding(0) var<uniform> uUniforms: Uniforms;

@vertex
fn vertex_main(in: VertexInput) -> VertexOutput
{
    var out: VertexOutput;
    out.position = vec4f(in.position, 0.0f, 1.0f);
    out.color = in.color;
    return out;
}

@fragment
fn fragment_main(in: VertexOutput) -> @location(0) vec4f
{
    return vec4f(uUniforms.color, 1.0f);
}
