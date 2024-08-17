struct CameraUniforms {
    projection: mat4x4f,
    view: mat4x4f,
}

struct ModelUniforms {
    model: mat4x4f,
}

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) color: vec3f,
    @location(3) uv: vec2f,
}

struct FragmentInput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
    @location(1) normal: vec3f,
    @location(2) uv: vec2f,
}

@group(0) @binding(0) var<uniform> uCamera: CameraUniforms;
@group(1) @binding(0) var<uniform> uModel: ModelUniforms;

@vertex
fn vertex_main(in: VertexInput) -> FragmentInput {
    var out: FragmentInput;
    out.position = uCamera.projection * uCamera.view * uModel.model * vec4f(in.position, 1.0);
    out.color = in.color;
    out.normal = (uModel.model * vec4f(in.normal, 0.0)).xyz;
    out.uv = in.uv;
    return out;
}

@group(1) @binding(1) var albedoTexture: texture_2d<f32>;
@group(1) @binding(2) var textureSampler: sampler;

@fragment
fn fragment_main(in: FragmentInput) -> @location(0) vec4f {
    // Compute shading
    let normal = normalize(in.normal);
    let lightDirection1 = vec3f(0.5, 0.1, -0.9);
    let lightDirection2 = vec3f(0.2, 0.3, 0.4);
    let lightColor1 = vec3f(1.0, 0.9, 0.6);
    let lightColor2 = vec3f(0.6, 0.9, 1.0);
    let shading1 = max(0.0, dot(lightDirection1, normal));
    let shading2 = max(0.0, dot(lightDirection2, normal));
    let shading = shading1 * lightColor1 + shading2 * lightColor2;

    // Sample texture
    let baseColor = textureSample(albedoTexture, textureSampler, in.uv).rgb;

    // Combine texture and lighting
    let color = baseColor * shading;

    return vec4f(color, 1.0);
}
