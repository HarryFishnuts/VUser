
/* ========== <vuser.c>							==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Master header file for user-interface.					*/
/* Includes all structs and defs, also all includes			*/

/* ========== INCLUDES							==========	*/
#include "vuser.h"
#include "vpanelshader.h"

/* ========== CORE FUNCTIONS					==========	*/
VUAPI void vUInitialize(void)
{
	vZeroMemory(&_vuser, sizeof(vUserInternals));
	_vuser.panelList = vCreateBuffer("vUPanel List", sizeof(vUPanel),
		MAX_PANELS, NULL, NULL);
}

VUAPI vPUPanelStyle vUCreatePanelStyle(vGColor fillColor, vGColor borderColor,
	vGColor textColor, float borderWidth, float buttonHoverScale, float buttonClickScale)
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
	style->buttonHoverScale = buttonHoverScale;
	style->buttonClickScale = buttonClickScale;
	
	vLogInfoFormatted(__func__, "Created new panel style %p.",
		style);
	
	return style;
}

VUAPI vPUPanel vUCreatePanelRect(vPUPanelStyle style, vGRect rect, vPGSkin skin);

VUAPI vPUPanel vUCreatePanelButton(vPUPanelStyle style, vGRect rect, vPGSkin skin);

VUAPI vPUPanel vUCreatePanelText(vPUPanelStyle style, vGRect rect, vUPanelTextFormat format,
	vPCHAR textPointer);
