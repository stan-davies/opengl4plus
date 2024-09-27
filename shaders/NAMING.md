Because these files are compiled from our own C++ code, we load them as well. This means that we can call these files whatever we want, with any name and file extension, and it's fine! However, there does seem to be various standards that different people use. Here are a few I have seen.

### `shader.glsl`
Very simple, makes a lot of sense. '*shader*' is replaced with the type of shader. Examples:
- `vertex.glsl` - a vertex shader
- `fragment.glsl` - a fragment shader

### `program.gl*s`
The '*\**' is replaced with a character describing the type of shader, '*program*' is replaced with the shader program that the shader will be attached to. Examples:
- `texturemap.glvs` - a vertex shader for a texture-mapping shader program
- `plaingeom.glfs` - a fragment shader for a plain geometry shader program

### `program.shader`
'*program*' is replaced with the shader program that the shader will be attached to, '*shader*' is replaced with (an often four character abbreviation of) the type of shader. Examples:
- `texturemap.vert` - a vertex shader for a texture-mapping shader program
- `plaingeom.frag` - a fragment shader for a plain geometry shader program

### which to use?
Different IDE's will pick up on different extensions, but I have configured both VScode and github to identify extensions by my explicity telling them what's what. I think that `program.shader` looks fairly good for now.