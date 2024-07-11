struct Uniforms {
    projection: mat4x4f,
    view: mat4x4f,
    model: mat4x4f,
}

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) color: vec3f,
}

struct FragmentInput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
    @location(1) normal: vec3f,
}

@group(0) @binding(0) var<uniform> uCamera: Uniforms;

@vertex
fn vertex_main(in: VertexInput) -> FragmentInput {
    var out: FragmentInput;
    out.position = uCamera.projection * uCamera.view * uCamera.model * vec4f(in.position, 1.0);
    out.color = in.color;
    out.normal = (uCamera.model * vec4f(in.normal, 0.0)).xyz;
    return out;
}

@fragment
fn fragment_main(in: FragmentInput) -> @location(0) vec4f {
    let normal = normalize(in.normal);

    let lightDirection1 = vec3f(0.5, -0.9, 0.1);
    let lightDirection2 = vec3f(0.2, 0.4, 0.3);
    let shading1 = max(0.0, dot(lightDirection1, normal));
    let shading2 = max(0.0, dot(lightDirection2, normal));
    let shading = shading1 + shading2;
    let color = in.color * shading;

    return vec4f(color, 1.0);
}
