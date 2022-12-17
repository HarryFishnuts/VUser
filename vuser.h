
/* ========== <vuser.h>							==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Master header file for user-interface.					*/
/* Includes all structs and defs, also all includes			*/

#ifndef _VUSER_INCLUDE_
#define _VUSER_INCLUDE_

/* ========== INCLUDES							==========	*/
#include "vcore.h"
#include "vgfx.h"


/* ========== DEFINITIONS						==========	*/
#ifdef VUSER_EXPORTS
#define VUAPI __declspec(dllexport)
#else
#define VUAPI __declspec(dllimport)
#endif

#define GUI_DEPTH			5.0
#define MAX_PANEL_STYLES	0x80
#define MAX_PANELS			0x200


/* ========== ENUMS								==========	*/
typedef enum vUPanelType
{
	vUPanelType_Rect,
	vUPanelType_Button,
	vUPanelType_Text
} vUPanelType;

typedef enum vUPanelTextFormat
{
	vUPanelTextFormat_LeftAligned,
	vUPanelTextFormat_RightAligned,
	vUPanelTextFormat_Centered
} vUPanelTextFormat;


/* ========== STRUCTS							==========	*/
typedef struct vUPanelStyle
{
	vGColor fillColor;
	vGColor borderColor;
	vGColor textColor;
	float   borderWidth;
	float	buttonHoverScale;
	float	buttonClickScale;
} vUPanelStyle, *vPUPanelStyle;

typedef struct vUPanel
{
	vUI8 layer;

	struct vUPanel* parent;

	vUPanelType panelType;
	vUPanelTextFormat textFormat;
	vPUPanelStyle style;

	vGRect  boundingBox;
	vPGSkin image;

	vPCHAR text;
	float  textSize;
} vUPanel, *vPUPanel;

typedef struct vUserInternals
{
	vPUPanelStyle panelStyleList[MAX_PANEL_STYLES];
	vUI32 panelStyleCount;

	vHNDL panelList;	/* fixed list of all panels */

	GLuint panelShaderMesh;
	GLuint panelShaderVertexArray;
	vPGShader panelShader;
	vPObject  panelObject;
	vPGRenderable panelRenderer;
} vUserInternals, *vPUserInternals;
vUserInternals _vuser;	/* INSTANCE */


/* ========== CORE FUNCTIONS					==========	*/
VUAPI void vUInitialize(void);
VUAPI vPUPanelStyle vUCreatePanelStyle(vGColor fillColor, vGColor borderColor,
	vGColor textColor, float borderWidth, float buttonHoverScale, float buttonClickScale);
VUAPI vPUPanel vUCreatePanelRect(vPUPanelStyle style, vGRect rect, vPGSkin skin);
VUAPI vPUPanel vUCreatePanelButton(vPUPanelStyle style, vGRect rect, vPGSkin skin);
VUAPI vPUPanel vUCreatePanelText(vPUPanelStyle style, vGRect rect, vUPanelTextFormat format,
	vPCHAR textPointer);

#endif
