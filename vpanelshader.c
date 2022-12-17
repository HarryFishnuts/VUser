
/* ========== <vpanelshader.c>					==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Internal panel shader logic								*/

/* ========== INCLUDES							==========	*/
#define GLEW_STATIC
#include "glew.h"
#include "vpanelshader.h"
#include <stdio.h>


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
"out vec2 f_textureUV;\n"
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
"out vec4 FragColor;"
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

		/* clear texture matrix */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		/* setup model matrix */
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(panel->boundingBox.left, panel->boundingBox.bottom,
			-2.0f);
		float panelWidth  = panel->boundingBox.right - panel->boundingBox.left;
		float panelHeight = panel->boundingBox.top - panel->boundingBox.bottom;
		glScalef(panelWidth, panelHeight, 1.0f);

		/* enable textures */
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		/* default to use completely white texture */
		glBindTexture(GL_TEXTURE_2D, _vuser.panelNoSkinTexture);

		/* if image exists for panel, use that */
		if (panel->skin != NULL)
			glBindTexture(GL_TEXTURE_2D, panel->skin->glHandle);

		/* no wrap texture */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		/* bind to buffer and vertex array */
		glBindBuffer(GL_ARRAY_BUFFER, _vuser.panelShaderMesh);
		glBindVertexArray(_vuser.panelShaderVertexArray);

		/* retrieve all data from gl matrix stack */
		GLfloat projectionMatrix[0x10];
		GLfloat modelMatrix[0x10];
		glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);

		/* apply uniform values */
		glUniform4fv(1, 1, &panel->style->borderColor);
		glUniformMatrix4fv(2, 1, GL_FALSE, projectionMatrix);
		glUniformMatrix4fv(3, 1, GL_FALSE, modelMatrix);

		/* draw outer box */
		glDrawArrays(GL_QUADS, 0, 4);

		/* transform model to draw inner box */
		float borderWidth = panel->style->borderWidth;
		glLoadIdentity();
		glTranslatef(panel->boundingBox.left + borderWidth,
			panel->boundingBox.bottom + borderWidth,
			GUI_DEPTH + ((float)panel->layer / 255.0f));
		glScalef(panelWidth - (borderWidth * 2.0f), 
			panelHeight - (borderWidth * 2.0f), 1.0f);

		/* retrieve all new data from gl matrix stack */
		glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);

		/* apply uniform values */
		glUniform4fv(1, 1, &panel->style->fillColor);
		glUniformMatrix4fv(2, 1, GL_FALSE, projectionMatrix);
		glUniformMatrix4fv(3, 1, GL_FALSE, modelMatrix);

		/* draw inner box */
		glDrawArrays(GL_QUADS, 0, 4);

		break;

	default:
		break;
	}
}


/* ========== SHADER FUNCTIONS					==========	*/
void vUPanel_shaderInitFunc(vPGShader shader, vPTR shaderData, vPTR input)
{
	/* init glew */
	glewInit();

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

	/* init noskin texture */
	glGenTextures(1, &_vuser.panelNoSkinTexture);
	glBindTexture(GL_TEXTURE_2D, _vuser.panelNoSkinTexture);
	vBYTE texData[4] = { 255, 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, ZERO, GL_RGBA, 1, 1, ZERO, GL_RGBA,
		GL_UNSIGNED_BYTE, texData);
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