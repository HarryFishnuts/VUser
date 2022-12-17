
/* ========== <vpanelshader.c>					==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Internal panel shader logic								*/

/* ========== INCLUDES							==========	*/
#define GLEW_STATIC
#include "glew.h"
#include "vpanelshader.h"


/* ========== SHADER SOURCE						==========	*/
static vPCHAR panelVertSrc =
"#version 460 core\n"
"\n"
"layout (location = 0) in vec2 v_position;\n"
"layout (location = 1) uniform vec4 v_color;\n"
"layout (location = 2) uniform mat4 v_projMatrix;\n"
"layout (location = 3) uniform mat4 v_modelMatrix;\n"
"\n"
"out vec4 f_color;\n"
"\n"
"void main()\n"
"{\n"
"\tf_color\t\t= v_color;\n"
"\tf_textureUV = v_position;\n"
"\tgl_Position = v_modelMatrix * v_projMatrix * vec4(v_position, 0.0, 1.0);\t\t\n"
"}\n"
"";

static vPCHAR panelFragSrc =
"#version 460 core\n"
"\n"
"in vec2 f_textureUV;\n"
"in vec4 f_color;\n"
"\n"
"uniform sampler2D f_texture;\n"
"\n"
"void main()\n"
"{\n"
"\tFragColor = texture(f_texture, f_textureUV) * f_color;\n"
"\n"
"\t/* dithering alogrithm */\n"
"\tif (FragColor.a <= 0.97)\n"
"\t{\n"
"\t\tif (FragColor.a <= 0.03) discard;\n"
"\t\tint x = int(gl_FragCoord.x);\n"
"\t\tint y = int(gl_FragCoord.y);\n"
"\n"
"\t\tif (FragColor.a < 0.5)\n"
"\t\t{\n"
"\t\t\tint discardInterval = int(1.0f / FragColor.a);\n"
"\t\t\tint step = x + (y * (discardInterval >> 1)) + (y * (discardInterval >> 3));\n"
"\t\t\tif (step % discardInterval != 0) discard;\n"
"\t\t}\n"
"\t\telse\n"
"\t\t{\n"
"\t\t\tint discardInterval = int(1.0f / (1.0f - FragColor.a));\n"
"\t\t\tint step = x + (y * (discardInterval >> 1)) + (y * (discardInterval >> 3));\n"
"\t\t\tif (step % discardInterval == 0) discard;\n"
"\t\t}\n"
"\t}\n"
"\n"
"\tFragColor.a = 1.0;\n"
"}\n"
"";


/* ========== HELPER FUNCTIONS					==========	*/
void UPanelShaderRenderIterateFunc(vHNDL hndl, vUI16 index,
	vPUPanel panel, vPTR input)
{
	/* switch render method based on panel type */
	switch (panel->panelType)
	{
	/* most basic case */
	case vUPanelType_Rect:

		/* setup model matrix */
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(panel->boundingBox.left, panel->boundingBox.bottom,
			GUI_DEPTH + ((float)panel->layer / 255.0f));
		glScalef(panel->boundingBox.right - panel->boundingBox.left,
			panel->boundingBox.top - panel->boundingBox.bottom, 1.0f);

		/* enable textures */
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);

		/* default to use flat skin texture */
		glBindTexture(GL_TEXTURE_2D, _vgfx.defaultShaderData.missingTexture);

		break;

	default:
		break;
	}
}


/* ========== SHADER FUNCTIONS					==========	*/
void vUPanel_shaderInitFunc(vPGShader shader, vPTR shaderData, vPTR input)
{
	/* gen array and buffer */
	glGenVertexArrays(1, &_vuser.panelShaderVertexArray);
	glBindVertexArray(_vuser.panelShaderVertexArray);

	glGenBuffers(1, &_vuser.panelShaderMesh);
	glBindBuffer(GL_ARRAY_BUFFER, _vuser.panelShaderMesh);
	float baseRect[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0} };
	glBufferData(GL_ARRAY_BUFFER, sizeof(baseRect), baseRect, GL_STATIC_DRAW);

	/* init vertex array */
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
}

void vUPanel_shaderRenderFunc(vPGShader shader, vPTR shaderdata, vPObject object,
	vPGRenderable renderable)
{
	/* loop all panel objects */
	vBufferIterate(_vuser.panelList, UPanelShaderRenderIterateFunc, NULL);
}

vPCHAR UGetPanelShaderVertexSource(void)
{
	return panelVertSrc;
}

vPCHAR UGetPanelShaderFragmentSource(void)
{
	return panelFragSrc;
}