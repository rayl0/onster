#include "onster.h"
#include "onster_render.h"

#include <stdio.h>

static render_layer *Layer = NULL;
static render_layer *Layer2 = NULL;
static render_layer *Layer3 = NULL;
static render_layer *Layer4 = NULL;
static render_layer *Layer5 = NULL;
static render_layer *Layer6 = NULL;
static render_layer *Layer7 = NULL;
static render_layer *Layer8 = NULL;
static render_layer *Layer9 = NULL;
static render_layer *Layer10 = NULL;
static render_layer *Layer11 = NULL;
static render_layer *Layer12 = NULL;
static render_layer *Layer13 = NULL;

static texture *Texture = NULL;
static texture *Texture2 = NULL;

void OnsterInit()
{
    fprintf(stderr, "Init from onster\n");

    RenderInit();

    m3 Projection = m3Ortho(0, 1024, 576, 0);
    RenderUpdateProjection(Projection);

    Layer = CreateRenderLayer();
    Layer2 = CreateRenderLayer();

    Texture = CreateTexture("./tiles.png");
    Texture2 = CreateTexture("./list.png");
}

static u32 Count = 0;

void OnsterUpdateAndRender(onster_input *Input, r32 dt)
{
    // NOTE(rajat): This should be at the top to avoid crashes and function must return after checking this
    if(Input->Buttons.Terminate.EndedDown)
    {
        DeleteRenderLayer(Layer);
        DeleteRenderLayer(Layer2);

        DeleteTextue(Texture);

        RenderShutdown();

        return;
    }

    if(Input->Buttons.A.EndedDown)
        fprintf(stderr, "Hello from onster\n");

    if(Input->Buttons.B.EndedDown)
        fprintf(stderr, "Hello from CD\n");

    if(Count == 0)
        OnsterInit();

    LayerPushQuad(Layer2, C3(0, 1, 1), 255, V4(150, 100, 100, 100));

    LayerPushQuad(Layer, C3(1, 1, 1), 255, V4(0, 0, 100, 100));
    LayerPushQuad(Layer, C3(0, 1, 0), 255, V4(100, 100, 100, 100));
    LayerPushQuad(Layer, C3(0, 1, 0), 255, V4(100, 100, 100, 100));

    LayerPushQuad(Layer2, C3(1, 1, 0), 255, V4(50, 0, 100, 100));
    LayerPushQuad(Layer2, C3(0, 1, 1), 255, V4(150, 100, 100, 100));

    v2 TextureBox;
    GetTextureBounds(Texture, &TextureBox);

    LayerPushTexture(Layer2, Texture, 255, V4(V2(150, 100), TextureBox));
    LayerPushTexture(Layer2, Texture2, 255, V4(V2(400, 100), V2(200, 200)));
    LayerPushTexture(Layer2, Texture2, 160, V4(V2(400, 200), V2(200, 200)));

    RenderLayer(Layer);
    RenderLayer(Layer2);

    Count++;
}
