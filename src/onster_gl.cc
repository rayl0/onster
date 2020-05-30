// NOTE(rajat): OpenGL implementation for the renderer
// NOTE(rajat): Shouldn't have platform specific dependenciess.

// TODO(rajat): Query gl buffer storage extension before using it
// TODO(rajat): GL Buffer storage alternative should be provided as fallback
// TODO(rajat): Use manual memory when memory sub system is finished or being used

#include "onster.h"
#include "onster_render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// TODO(rajat): Remove this and move this to cmake configuraton specific

#ifdef GL_ES
#include <GLES3/g3.h>
#include <GLES3/gl3ext.h>
#else
#include <glad/glad.h>
#endif

#include <malloc.h>

// TODO(rajat): This should be lowered or increased depending on the needs, we don't
// know the needs yet
#define MAX_QUADS_PER_LAYER 500

struct gl_ctx
{
    u32 Shader;
    s32 MaxTextureSlots;

    struct gl_state
    {
        u32 BoundVertexBuffer;
        u32 BoundIndexBuffer;
        u32 BoundVertexArray;

        s32 *BoundTextureMap; // TODO(rajat): Empty slotes represent a texture.
    }GLState;
}GLContext = {};

// TODO(rajat): I think, it is always a good idea to have your shaders inlined.
// TODO(rajat): Take care of the GLSL version for different platforms

const char *VertexShaderSource = R"(
#version 420

#if GL_ES
precision mediump float;
#endif

layout(location = 0) in vec4 pos_texcoords;
layout(location = 1) in vec4 in_color;
layout(location = 2) in float tex_index;

out vec3 tex_params;
out vec4 color;

// NOTE(rajat): It is easier to optimize for simd with mat4x4.
uniform mat3 view_proj;

void main()
{
     gl_Position = vec4((view_proj * vec3(pos_texcoords.xy, 1)).xy, 0.0f, 1.0f);
     tex_params = vec3(pos_texcoords.zw, tex_index);
     color = in_color;
}
)";

const char *FragmentShaderSource = R"(
#version 420

#if GL_ES
precision mediump float;
#endif

out vec4 out_color;

in vec4 color;
in vec3 tex_params;

uniform sampler2D samples[%i];

void main()
{
   int tex_index = int(tex_params.z);

   if(tex_index < 0)
   out_color = color;
   else
   out_color = color * texture(samples[tex_index], tex_params.xy);
}
)";

void
RenderInit()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &GLContext.MaxTextureSlots);

    char *Buffer = (char *)malloc(strlen(FragmentShaderSource) + 1);
    sprintf(Buffer, FragmentShaderSource, GLContext.MaxTextureSlots);

    u32 FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &Buffer, NULL);
    glCompileShader(FragmentShader);

    char TempBuffer[2048];

    int Err;
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Err);

    if(!Err)
    {
        glGetShaderInfoLog(FragmentShader, 2048, NULL, TempBuffer);

        TODO_LOGGING;
        printf("GL Shader Error:\n\t<Fragment Shader>\n%s\n", TempBuffer);
    }

    free(Buffer);

    u32 VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
    glCompileShader(VertexShader);

    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Err);

    if(!Err)
    {
        glGetShaderInfoLog(VertexShader, 2048, NULL, TempBuffer);

        TODO_LOGGING;
        printf("GL Shader Error:\n\t<Vertex Shader>\n%s\n", TempBuffer);
    }

    GLContext.Shader = glCreateProgram();

    glAttachShader(GLContext.Shader, VertexShader);
    glAttachShader(GLContext.Shader, FragmentShader);

    glLinkProgram(GLContext.Shader);
    glGetProgramiv(GLContext.Shader, GL_LINK_STATUS, &Err);

    if(!Err)
    {
        glGetProgramInfoLog(GLContext.Shader, 2048, NULL, TempBuffer);

        TODO_LOGGING;
        printf("GL Error:\n\t<GL Program>\n%s\n", TempBuffer);
    }

    glUseProgram(GLContext.Shader);

    MEMORY_LEAK;
    s32 *Samples = (s32*)malloc(sizeof(s32) * GLContext.MaxTextureSlots);

    for(int i = 0; i < GLContext.MaxTextureSlots; ++i)
    {
        Samples[i] = i;
    }

    s32 SamplerArrayLoc = glGetUniformLocation(GLContext.Shader, "samples");
    glUniform1iv(SamplerArrayLoc, GLContext.MaxTextureSlots, Samples);

    free(Samples);
}

void
RenderShutdown()
{
    glDeleteProgram(GLContext.Shader);
}

typedef struct texture
{
    u32 Id;
    s32 Width;
    s32 Height;
    s32 Channels;
}texture;

texture
*CreateTexture(const char *FileName)
{
    MEMORY_LEAK;
    texture *Texture = (texture*)malloc(sizeof(texture));

    TODO_LOGGING; // NOTE(rajat): It may not be necessary to log here
    Assert(Texture != NULL);

    s32 w, h, n;
    ubyte *Pixels = stbi_load(FileName, &w, &h, &n, 0);

    Texture->Width = w;
    Texture->Height = h;
    Texture->Channels = n;

    glGenTextures(1, &Texture->Id);
    glBindTexture(GL_TEXTURE_2D, Texture->Id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    s32 Format = GL_RGBA;
    if(n == 3)
    {
        Format = GL_RGB;
    }

    // STUDY(rajat): Texture swizlling on 30 May, 2020 at night at my bed room.
    glTexImage2D(GL_TEXTURE_2D, 0, Format, w, h, 0, Format, GL_UNSIGNED_BYTE, Pixels);

    return Texture;
}

void
DeleteTextue(texture *Texture)
{
    glDeleteTextures(1, &Texture->Id);

    MEMORY_LEAK;
    free(Texture);
}

void
GetTextureBounds(texture *Texture, v2 *Bounds)
{
    Assert((Texture != NULL) && (Bounds != NULL));

    Bounds->x = Texture->Width;
    Bounds->y = Texture->Height;
}

// NOTE(rajat): Currently each render_layer is a single draw call candidate,
// As we would have multiple render_layers, so we could have something simpler
// instead of caching vertex data for multiple draw calls for a single layer.
typedef struct render_layer
{
    u32 VertexArray;
    u32 VertexBuffer;
    u32 IndexBuffer;

    r32 *VertexBufferPointer;
    r32 *VertexBufferCurrent;
    u32 VertexBufferOffset;
    u32 VertexBufferSize;

    u32 *IndexBufferPointer;
    u32 *IndexBufferCurrent;
    u32 IndexBufferOffset;
    u32 IndexBufferSize;

    u32 ElementAdvance;
    u32 VertexCount;

    u32 *TextureMap;
    u32 NumTextures;
}render_layer;

render_layer
*CreateRenderLayerWithNumQuadReversed(u32 NumQuads)
{
    MEMORY_LEAK;

    // NOTE(rajat): Beware of these kinds of bugs, they are really annoying
    /******** render_layer *NewLayer = (render_layer*)malloc(sizeof(NewLayer)); ***********/
    // sizeof(NewLayer) == 8 which is not the size of render_layer itself
    // so memory gets overwritten, and you try very hard to pin point the bug
    // becuase sometimes the app crashes or sometimes doesn't, also gdb has
    // catched it after any iterations.

    render_layer *NewLayer = (render_layer*)malloc(sizeof(render_layer));

    // TODO(rajat): Replace numeric contants with pound defines
    NewLayer->VertexBufferSize = sizeof(r32) * NumQuads * 9 * 4;
    NewLayer->VertexBufferOffset = 0;

    NewLayer->IndexBufferSize = sizeof(u32) * NumQuads * 6;
    NewLayer->IndexBufferOffset = 0;

    NewLayer->ElementAdvance = 0;
    NewLayer->VertexCount = 0;

    MEMORY_LEAK;
    NewLayer->TextureMap = (u32*)malloc(sizeof(u32) * GLContext.MaxTextureSlots);
    NewLayer->NumTextures = 0;

    glGenVertexArrays(1, &NewLayer->VertexArray);
    glBindVertexArray(NewLayer->VertexArray);

    glGenBuffers(1, &NewLayer->VertexBuffer);
    glGenBuffers(1, &NewLayer->IndexBuffer);

    GLbitfield Flags = GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    GLbitfield CreateFlags = Flags | GL_DYNAMIC_STORAGE_BIT;

    glBindBuffer(GL_ARRAY_BUFFER, NewLayer->VertexBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, NewLayer->VertexBufferSize, NULL, CreateFlags);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NewLayer->IndexBuffer);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, NewLayer->IndexBufferSize, NULL, CreateFlags);

    // TODO(rajat): It may be a good idea to use manual syncing for persistent buffers
    // to reduce overhead

    NewLayer->VertexBufferPointer = (r32*)glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                                           NewLayer->VertexBufferSize, Flags);

    NewLayer->IndexBufferPointer = (u32*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
                                                          NewLayer->IndexBufferSize, Flags);

    NewLayer->VertexBufferCurrent = NewLayer->VertexBufferPointer;
    NewLayer->IndexBufferCurrent = NewLayer->IndexBufferPointer;

    return NewLayer;
}

render_layer
*CreateRenderLayer()
{
    return CreateRenderLayerWithNumQuadReversed(MAX_QUADS_PER_LAYER);
}

void
DeleteRenderLayer(render_layer *Layer)
{
    glUnmapBuffer(Layer->VertexBuffer);
    glUnmapBuffer(Layer->IndexBuffer);

    glDeleteBuffers(1, &Layer->IndexBuffer);
    glDeleteBuffers(1, &Layer->VertexBuffer);

    glDeleteVertexArrays(1, &Layer->VertexArray);

    free(Layer->TextureMap);
    free(Layer);
}

void
LayerPushQuad(render_layer *Layer, v3 Color, r32 Alpha, v4 DestRect)
{
    Assert(Layer != NULL);

    int Index = -1.0f;

    m3 Model = M3(1.0f);
    Model = m3Translate(Model, DestRect.min);
    Model = m3Scale(Model, DestRect.max);

    // TODO(rajat): Optimize using SIMD, Matrix vector multiplication.
    v3 VertexData[4] = {
        {0, 0, 1},
        {0, 1, 1},
        {1, 1, 1},
        {1, 0, 1}
    };

    for(int i = 0; i < 4; ++i)
    {
        VertexData[i] = VertexData[i] * Model;
    }

    r32 OutVertexData[4 * 9] = {
        VertexData[0].x, VertexData[0].y, 0, 0, Color.r, Color.y, Color.z, Alpha / 255.0f, Index,
        VertexData[1].x, VertexData[1].y, 0, 0, Color.r, Color.y, Color.z, Alpha / 255.0f, Index,
        VertexData[2].x, VertexData[2].y, 0, 0, Color.r, Color.y, Color.z, Alpha / 255.0f, Index,
        VertexData[3].x, VertexData[3].y, 0, 0, Color.r, Color.y, Color.z, Alpha / 255.0f, Index,
    };

    for(int i = 0; i < (4 * 9); ++i, ++Layer->VertexBufferCurrent)
    {
        *Layer->VertexBufferCurrent = OutVertexData[i];
    }

    Layer->VertexBufferOffset += 4 * 9;

    u32 OutIndexData[6] = {
        Layer->ElementAdvance, Layer->ElementAdvance + 1, Layer->ElementAdvance + 2,
        Layer->ElementAdvance + 2, Layer->ElementAdvance + 3, Layer->ElementAdvance
    };

    for(int i = 0; i < 6; ++i, ++Layer->IndexBufferCurrent)
    {
        *Layer->IndexBufferCurrent = OutIndexData[i];
    }

    Layer->IndexBufferOffset += 6;
    Layer->ElementAdvance += 4;
    Layer->VertexCount += 6; // NOTE(rajat): This may be redundent to have this, as IndexBufferOffset will same as thsi
}

void LayerPushTexture(render_layer *Layer, texture *Texture, r32 Alpha, v4 DestRect);
void LayerPushTextureColor(render_layer *Layer, texture *Texture, v3 Color, r32 Alpha, v4 DestRect);
void LayerPushTextureFrame(render_layer *Layer, texture *Texture, v4 SrcRect, r32 Alpha, v4 DestRect);
void LayerPushTextureColorFrame(render_layer *Layer, texture *Texture, v4 SrcRect, v3 Color, r32 Alpha, v4 DestRect);

void
RenderLayer(render_layer *Layer)
{
    Assert(Layer != NULL);

    glUseProgram(GLContext.Shader);

    // TODO(rajat): Remove this after adding ViewProjection update functions in the API
    m3 Projection = m3Ortho(0, 800, 600, 0);

    u32 ViewProjLocation = glGetUniformLocation(GLContext.Shader, "view_proj");
    glUniformMatrix3fv(ViewProjLocation, 1, GL_FALSE, Projection.Data);

    glBindVertexArray(Layer->VertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Layer->IndexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Layer->VertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(r32) * 9, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(r32) * 9, (void*)(sizeof(r32) * 4));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(r32) * 9, (void*)(sizeof(r32) * 8));

    glDrawElements(GL_TRIANGLES, Layer->VertexCount, GL_UNSIGNED_INT, NULL);

    Layer->VertexBufferCurrent = Layer->VertexBufferPointer;
    Layer->VertexBufferOffset = 0;

    Layer->IndexBufferCurrent = Layer->IndexBufferPointer;
    Layer->IndexBufferOffset = 0;
    Layer->ElementAdvance = 0;
    Layer->VertexCount = 0;

    Layer->NumTextures = 0;
}
