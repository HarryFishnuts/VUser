
/* ========== <vuser.c>							==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Master header file for user-interface.					*/
/* Includes all structs and defs, also all includes			*/

/* ========== INCLUDES							==========	*/
#define GLEW_STATIC
#include "glew.h"
#include "vuser.h"
#include "vpanelshader.h"


/* ========== CORE FUNCTIONS					==========	*/
VUAPI void vUInitialize(void)
{
	vZeroMemory(&_vuser, sizeof(vUserInternals));
	_vuser.panelList = vCreateBuffer("vUPanel List", sizeof(vUPanel),
		MAX_PANELS, NULL, NULL);
	
	/* create panel shader */
	_vuser.panelShader = 
		vGCreateShader(vUPanel_shaderInitFunc, vUPanel_shaderRenderFunc,
		NULL, ZERO,
		UGetPanelShaderVertexSource(), UGetPanelShaderFragmentSource(), NULL);

	/* create panel object and renderer */
	_vuser.panelObject = vCreateObject(NULL);
	_vuser.panelRenderer = vGCreateRenderable(_vuser.panelObject,
		vCreateTransform(vCreatePosition(0.0f, 0.0f), 0.0f, 1.0f),
		_vuser.panelShader, NULL, vGCreateRectCentered(0.1f, 0.1f), NULL);
}

VUAPI vGRect vUCreateRectCenteredOffset(vPosition offset, float width, float height)
{
	vGRect rect = vGCreateRectCentered(width, height);
	rect.bottom += offset.y;
	rect.top    += offset.y;
	rect.left   += offset.x;
	rect.right  += offset.x;
	return rect;
}

VUAPI vGRect vUCreateRectExpanded(vGRect reference, float expansion)
{
	vGRect newRect;
	newRect.bottom = reference.bottom - expansion;
	newRect.top = reference.top + expansion;
	newRect.left = reference.left - expansion;
	newRect.right = reference.right + expansion;
	return newRect;
}


VUAPI vPosition vUScreenToPanelSpace(vPosition screenPos)
{
	_vPGInternals pvgfx = vGGetInternals();
	/* get window dimensions */
	float windowHeight = pvgfx->window.dimensions.bottom - pvgfx->window.dimensions.top;
	float windowWidth = pvgfx->window.dimensions.right - pvgfx->window.dimensions.left;

	/* map to panel space */
	float aspect = windowWidth / windowHeight;
	float panelX = (screenPos.x / (windowWidth * (1.0f / (2.0f * aspect)))) - aspect;
	float panelY = (screenPos.y / (windowHeight * 0.5f)) - 1.0f;

	return vCreatePosition(panelX, panelY);
}

VUAPI vPosition vUPanelToScreenSpace(vPosition panelPos)
{
	float screenX = panelPos.x;
	float screenY = panelPos.y;

	_vPGInternals pvgfx = vGGetInternals();

	/* get window dimensions */
	float windowHeight = pvgfx->window.dimensions.bottom - pvgfx->window.dimensions.top;
	float windowWidth = pvgfx->window.dimensions.right - pvgfx->window.dimensions.left;

	/* map to screenspace */
	float aspect = windowWidth / windowHeight;
	float inverseaspect = windowHeight / windowWidth;
	screenX = (screenX + aspect) * (windowWidth * 0.5f * inverseaspect);
	screenY = (screenY + 1.0f) * (windowHeight * 0.5f);

	return vCreatePosition(screenX, screenY);
}

VUAPI vPosition vUMouseToPanelSpace(void)
{
	return vUScreenToPanelSpace(vGScreenSpaceMousePos());
}

VUAPI vBOOL vUIsMouseOverPanel(vPUPanel panel)
{
	vPosition mousePos = vUMouseToPanelSpace();
	return
		(mousePos.x < panel->boundingBox.right &&
		 mousePos.x > panel->boundingBox.left  &&
		 mousePos.y < panel->boundingBox.top   &&
		 mousePos.y > panel->boundingBox.bottom);
}


VUAPI vPUPanelStyle vUCreatePanelStyle(vGColor fillColor, vGColor borderColor,
	vGColor textColor, float borderWidth, float buttonHoverScale, float buttonClickScale,
	vPUPanelMouseBehavior mouseBehavior)
{
	if (_vuser.panelStyleCount >= MAX_PANEL_STYLES)
	{
		vLogError(__func__, "Exceeded max style count.");
		return NULL;
	}

	vPUPanelStyle style = _vuser.panelStyleList + _vuser.panelStyleCount;
	style->fillColor = fillColor;
	style->borderColor = borderColor;
	style->textColor = textColor;
	style->borderWidth = borderWidth;
	style->buttonHoverWidth = buttonHoverScale;
	style->buttonClickWidth = buttonClickScale;
	if (mouseBehavior != NULL)
		vMemCopy(&style->mouseBhv, mouseBehavior, sizeof(vUPanelMouseBehavior));
	
	vLogInfoFormatted(__func__, "Created new panel style %p.",
		style);
	
	return style;
}

VUAPI vPUPanel vUCreatePanelRect(vPUPanelStyle style, vGRect rect, vPGSkin skin)
{
	vPUPanel panel = vBufferAdd(_vuser.panelList, NULL);
	panel->panelType = vUPanelType_Rect;
	panel->style = style;
	panel->boundingBox = rect;
	panel->skin = skin;
	return panel;
}

VUAPI vPUPanel vUCreatePanelButton(vPUPanelStyle style, vGRect rect, vPGSkin skin)
{
	vPUPanel panel = vUCreatePanelRect(style, rect, skin);
	panel->panelType = vUPanelType_Button;
	return panel;
}

VUAPI vPUPanel vUCreatePanelText(vPUPanelStyle style, vGRect rect, vUPanelTextFormat format,
	vPCHAR textPointer);
