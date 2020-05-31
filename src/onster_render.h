#pragma once

// NOTE(rajat): There could be even an render command implementation later
// but that is not going to effect this that much, we can always implement
// renderer beneath this API.

#include "onster.h"
#include "onster_math.h"

// TODO(rajat): Add matrix transformation functions i.e. ViewProjection update
// functions for handling change in screen dimensions.

extern void RenderInit();
extern void RenderShutdown();
extern void RenderUpdateProjection(m3 Projection);

typedef struct texture texture;

extern texture *CreateTexture(char *FileName);
extern void DeleteTextue(texture *Texture);
extern void GetTextureBounds(texture *Texture, v2 *Bounds);

typedef struct render_layer render_layer;

extern render_layer *CreateRenderLayer();
extern render_layer *CreateRenderLayerWithNumQuadReversed(u32 NumQuads);
extern void DeleteRenderLayer(render_layer *Layer);

extern void LayerPushQuad(render_layer *Layer, v3 Color, r32 Alpha, v4 DestRect);
extern void LayerPushTexture(render_layer *Layer, texture *Texture, r32 Alpha, v4 DestRect);
extern void LayerPushTextureColor(render_layer *Layer, texture *Texture, v3 Color, r32 Alpha, v4 DestRect);
extern void LayerPushTextureFrame(render_layer *Layer, texture *Texture, v4 SrcRect, r32 Alpha, v4 DestRect);
extern void LayerPushTextureColorFrame(render_layer *Layer, texture *Texture, v4 SrcRect, v3 Color, r32 Alpha, v4 DestRect);

extern void RenderLayer(render_layer *Layer);
