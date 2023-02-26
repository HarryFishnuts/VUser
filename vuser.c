
/* ========== <vuser.c>							==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Master header file for user-interface.					*/
/* Includes all structs and defs, also all includes			*/

/* ========== INCLUDES							==========	*/
#define GLEW_STATIC
#include "glew.h"
#include "vuser.h"
#include "vpanelshader.h"
#include <stdio.h>
#include <Windows.h>


/* ========== CORE FUNCTIONS					==========	*/
VUAPI void vUInitialize(void)
{
	vZeroMemory(&_vuser, sizeof(vUserInternals));
	_vuser.panelList = vCreateBuffer("vUPanel List", sizeof(vUPanel),
		MAX_PANELS, NULL, NULL);

	InitializeCriticalSection(&_vuser.lock);
	
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

VUAPI void vULock(void) {
	EnterCriticalSection(&_vuser.lock);
}

VUAPI void vUUnlock(void) {
	LeaveCriticalSection(&_vuser.lock);
}

VUAPI float  vUGetRectWidth(vGRect rect) {
	return rect.right - rect.left;
}

VUAPI float  vUGetRectHeight(vGRect rect) {
	return rect.top - rect.bottom;
}

VUAPI float  vUGetRectAspect(vGRect rect) {
	return vUGetRectWidth(rect) / vUGetRectHeight(rect);
}

VUAPI vGRect vUMoveRect(vGRect rect, float mx, float my) {
	return vGCreateRect(
		rect.left + mx,
		rect.right + mx,
		rect.bottom + my,
		rect.top + my
	);
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

VUAPI vGRect vUCreateRectAlignedBorder(vURectAlignment alignment,
	vGRect target, float border) {
	float wAspect = vGGetWindowClientAspect();
	return
		vUCreateRectAlignedIn(
			vGCreateRectCentered(wAspect * 2, 2.0),
			alignment,
			target,
			border
		);
}

VUAPI vGRect vUCreateRectAlignedIn(vGRect alignRect, vURectAlignment alignment,
	vGRect target, float border) {
	alignRect = vUCreateRectExpanded(alignRect, -border);
	switch (alignment)
	{
		/* align top face with rect */
	case vURectAlignment_Top:
		return vGCreateRect(
			target.left,
			target.right,
			alignRect.top - vUGetRectHeight(target),
			alignRect.top
		);
		/* align bottom face with rect */
	case vURectAlignment_Bottom:
		return vGCreateRect(
			target.left,
			target.right,
			alignRect.bottom,
			alignRect.bottom + vUGetRectHeight(target)
		);
		/* align left face with rect */
	case vURectAlignment_Left:
		return vGCreateRect(
			alignRect.left,
			alignRect.left + vUGetRectWidth(target),
			target.bottom,
			target.top
		);
		/* align right face with rect */
	case vURectAlignment_Right:
		return vGCreateRect(
			alignRect.right - vUGetRectWidth(target),
			alignRect.right,
			target.bottom,
			target.top
		);

	default:
		break;
	}

	/* on fail, return target */
	return target;
}

VUAPI vGRect vUCreateRectAlignedOut(vGRect alignRect, vURectAlignment alignment,
	vGRect target, float border) {
	alignRect = vUCreateRectExpanded(alignRect, border);
	switch (alignment)
	{
	/* align top face with rect */
	case vURectAlignment_Top:
		return vGCreateRect(
			target.left,
			target.right,
			alignRect.bottom - vUGetRectHeight(target),
			alignRect.bottom
		);
	/* align bottom face with rect */
	case vURectAlignment_Bottom:
		return vGCreateRect(
			target.left,
			target.right,
			alignRect.top,
			alignRect.top + vUGetRectHeight(target)
		);
	/* align left face with rect */
	case vURectAlignment_Left:
		return vGCreateRect(
			alignRect.right,
			alignRect.right + vUGetRectWidth(target),
			target.bottom,
			target.top
		);
	/* align right face with rect */
	case vURectAlignment_Right:
		return vGCreateRect(
			alignRect.left - vUGetRectWidth(target),
			alignRect.left,
			target.bottom,
			target.top
		);

	default:
		break;
	}

	/* on fail, return target */
	return target;
}

VUAPI vGRect vUCreateRectFromTable(vGRect tableRect, vUI32 xDivisions, vUI32 yDivisions,
	float elementBorder, vUI32 xPos, vUI32 yPos) {
	/* calculate box size */
	float boxWidth  = vUGetRectWidth(tableRect) / (float)xDivisions;
	float boxHeight = vUGetRectHeight(tableRect) / (float)yDivisions;

	/* make box based on x and y pos */
	vGRect boxRect = vGCreateRect(
		boxWidth * xPos,
		boxWidth * (xPos + 1),
		boxHeight * (yPos),
		boxHeight * (yPos + 1)
	);
	boxRect =
		vUMoveRect(boxRect, tableRect.left, tableRect.bottom);
	boxRect =
		vUCreateRectExpanded(boxRect, -elementBorder);

	return boxRect;
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
	EnterCriticalSection(&_vuser.lock);
	vPosition mousePos = vUMouseToPanelSpace();
	BOOL state = (mousePos.x < panel->boundingBox.right&&
		mousePos.x > panel->boundingBox.left &&
		mousePos.y < panel->boundingBox.top&&
		mousePos.y > panel->boundingBox.bottom);
	LeaveCriticalSection(&_vuser.lock);
	return state;
		
}

VUAPI vBOOL vUIsMouseClickingPanel(vPUPanel panel) {
	EnterCriticalSection(&_vuser.lock);
	volatile SHORT keyState = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	BOOL state = (vUIsMouseOverPanel(panel) && keyState);
	LeaveCriticalSection(&_vuser.lock);
	return state;
}


VUAPI vPUPanelStyle vUCreatePanelStyle(vGColor fillColor, vGColor borderColor,
	vGColor textColor, float borderWidth, float buttonHoverScale, float buttonClickScale)
{
	EnterCriticalSection(&_vuser.lock);

	if (_vuser.panelStyleCount >= MAX_PANEL_STYLES)
	{
		vLogError(__func__, "Exceeded max style count.");
		LeaveCriticalSection(&_vuser.lock);
		return NULL;
	}

	vPUPanelStyle style = &_vuser.panelStyleList[_vuser.panelStyleCount];
	style->fillColor = fillColor;
	style->borderColor = borderColor;
	style->textColor = textColor;
	style->borderWidth = borderWidth;
	style->buttonHoverWidth = buttonHoverScale;
	style->buttonClickWidth = buttonClickScale;
	style->tintSkinFillColor = FALSE;
	
	vLogInfoFormatted(__func__, "Created new panel style %p.",
		style);
	
	_vuser.panelStyleCount++;

	LeaveCriticalSection(&_vuser.lock);
	return style;
}

VUAPI vPUPanel vUCreatePanelRect(vPUPanelStyle style, vGRect rect, vPGSkin skin)
{
	EnterCriticalSection(&_vuser.lock);

	vPUPanel panel = vBufferAdd(_vuser.panelList, NULL);
	panel->panelType = vUPanelType_Rect;
	panel->style = style;
	panel->boundingBox = rect;
	panel->skin = skin;

	LeaveCriticalSection(&_vuser.lock);
	return panel;
}

VUAPI vPUPanel vUCreatePanelButton(vPUPanelStyle style, vGRect rect, vPGSkin imageSkin,
	vPUPanelMouseBehavior targetMouseBhv)
{
	EnterCriticalSection(&_vuser.lock);

	vPUPanel panel = vUCreatePanelRect(style, rect, imageSkin);
	panel->panelType = vUPanelType_Button;
	if (targetMouseBhv != NULL)
		vMemCopy(&panel->mouseBhv, targetMouseBhv, sizeof(vUPanelMouseBehavior));

	LeaveCriticalSection(&_vuser.lock);
	return panel;
}

VUAPI vPUPanel vUCreatePanelText(vPUPanelStyle style, vGRect rect, vUPanelTextFormat format,
	float textSize, vPCHAR textPointer)
{
	EnterCriticalSection(&_vuser.lock);

	vPUPanel panel = vUCreatePanelRect(style, rect, _vuser.defaultTextSkin);
	InitializeCriticalSection(&panel->textLock);
	panel->panelType	= vUPanelType_Text;
	panel->textFormat	= format;
	panel->text			= textPointer;
	panel->textSize		= textSize;

	LeaveCriticalSection(&_vuser.lock);
	return panel;
}

VUAPI void vUPanelTextLock(vPUPanel panel) {
	EnterCriticalSection(&panel->textLock);
}

VUAPI void vUPanelTextUnlock(vPUPanel panel) {
	LeaveCriticalSection(&panel->textLock);
}

VUAPI void vUDestroyPanel(vPUPanel panel) {
	EnterCriticalSection(&_vuser.lock);

	if (panel->panelType == vUPanelType_Text) {
		DeleteCriticalSection(&panel->textLock);
	}
	vBufferRemove(_vuser.panelList, panel);

	LeaveCriticalSection(&_vuser.lock);
}

VUAPI void vUDestroyPanelAndSkin(vPUPanel panel) {
	EnterCriticalSection(&_vuser.lock);
	vGDestroySkin(panel->skin);
	vUDestroyPanel(panel);
	LeaveCriticalSection(&_vuser.lock);
}
