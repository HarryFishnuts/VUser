
/* ========== <vpanelshader.c>					==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Internal panel shader logic								*/

/* ========== INCLUDES							==========	*/
#define GLEW_STATIC
#include "glew.h"
#include "vpanelshader.h"


/* ========== SHADER SOURCE						==========	*/
static vPCHAR panelVertSrc = "";
static vPCHAR panelFragSrc = "";

/* ========== SHADER FUNCTIONS					==========	*/
void vUPanel_shaderRenderFunc(vPGShader shader, vPTR shaderdata, vPObject object,
	vPGRenderable renderable)
{

}

vPCHAR UGetPanelShaderVertexSource(void)
{
	return panelVertSrc;
}

vPCHAR UGetPanelShaderFragmentSource(void)
{
	return panelFragSrc;
}