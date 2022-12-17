
/* ========== <vpanelshader.h>					==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Internal panel shader logic								*/

#ifndef _VUSER_INTERNAL_PANELSHADER_INCLUDE_
#define _VUSER_INTERNAL_PANELSHADER_INCLUDE_ 

/* ========== INCLUDES							==========	*/
#include "vuser.h"


/* ========== SHADER FUNCTIONS					==========	*/
void vUPanel_shaderRenderFunc(vPGShader shader, vPTR shaderdata, vPObject object,
	vPGRenderable renderable);
vPCHAR UGetPanelShaderVertexSource(void);
vPCHAR UGetPanelShaderFragmentSource(void);

#endif
