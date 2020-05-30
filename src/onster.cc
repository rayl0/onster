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

void OnsterInit()
{
    fprintf(stderr, "Init from onster\n");

    RenderInit();

    Layer = CreateRenderLayer();
    Layer2 = CreateRenderLayer();
    Layer3 = CreateRenderLayer();
    Layer4 = CreateRenderLayer();
    Layer5 = CreateRenderLayer();
    Layer6 = CreateRenderLayer();
    Layer7 = CreateRenderLayer();
    Layer8 = CreateRenderLayer();
    Layer9 = CreateRenderLayer();
    Layer10 = CreateRenderLayer();
    Layer11 = CreateRenderLayer();
    Layer12 = CreateRenderLayer();
    Layer13 = CreateRenderLayer();
}

static u32 Count = 0;

void OnsterUpdateAndRender(onster_input *Input, r32 dt)
{
    // NOTE(rajat): This should be at the top to avoid crashes and function must return after checking this
    if(Input->Buttons.Terminate.EndedDown)
    {
        DeleteRenderLayer(Layer);
        DeleteRenderLayer(Layer2);
        DeleteRenderLayer(Layer3);
        DeleteRenderLayer(Layer4);
        DeleteRenderLayer(Layer5);
        DeleteRenderLayer(Layer6);
        DeleteRenderLayer(Layer7);
        DeleteRenderLayer(Layer8);
        DeleteRenderLayer(Layer9);
        DeleteRenderLayer(Layer10);
        DeleteRenderLayer(Layer11);
        DeleteRenderLayer(Layer12);
        DeleteRenderLayer(Layer13);

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

    LayerPushQuad(Layer3, C3(1, 1, 0), 255, V4(100, 0, 100, 100));

    LayerPushQuad(Layer2, C3(1, 1, 0), 255, V4(50, 0, 100, 100));
    LayerPushQuad(Layer2, C3(0, 1, 1), 255, V4(150, 100, 100, 100));

    LayerPushQuad(Layer3, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
    LayerPushQuad(Layer3, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer4, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer4, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer5, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer5, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer6, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer6, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer7, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer7, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer8, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer8, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer9, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer9, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer10, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer10, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer11, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer11, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer12, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer12, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer13, C3(0, 1, 1), 255, V4(200, 100, 100, 100));
LayerPushQuad(Layer13, C3(0, 1, 1), 255, V4(200, 100, 100, 100));


    RenderLayer(Layer3);
    RenderLayer(Layer2);
    RenderLayer(Layer);

    RenderLayer(Layer4);
    RenderLayer(Layer5);
    RenderLayer(Layer6);
    RenderLayer(Layer7);
    RenderLayer(Layer8);
    RenderLayer(Layer9);
    RenderLayer(Layer10);
    RenderLayer(Layer11);
    RenderLayer(Layer12);
    RenderLayer(Layer13);

    Count++;
}
