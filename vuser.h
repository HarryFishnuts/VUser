
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

#define GUI_DEPTH			-5.0
#define MAX_PANEL_STYLES	0x80
#define MAX_PANELS			0x200


/* ========== CALLBACKS							==========	*/
typedef (*vPFUPANELONMOUSEOVER)(struct vUPanel* panel);
typedef (*vPFUPANELONMOUSEAWAY)(struct vUPanel* panel);
typedef (*vPFUPANELMOUSEOVER)(struct vUPanel* panel);
typedef (*vPFUPANELONMOUSECLICK)(struct vUPanel* panel);
typedef (*vPFUPANELONMOUSEUNCLICK)(struct vUPanel* panel);
typedef (*vPFUPANELMOUSECLICK)(struct vUPanel* panel);


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
	vUPanelTextFormat_Centered,
	vUPanelTextFormat_CenteredComplete
} vUPanelTextFormat;


/* ========== STRUCTS							==========	*/
typedef struct vUPanelMouseBehavior
{
	vPFUPANELMOUSEOVER  mouseOverFunc;
	vPFUPANELMOUSECLICK mouseClickFunc;

	vPFUPANELONMOUSEOVER    onMouseOverFunc;
	vPFUPANELONMOUSEAWAY    onMouseAwayFunc;
	vPFUPANELONMOUSECLICK   onMouseClickFunc;
	vPFUPANELONMOUSEUNCLICK onMouseUnclickFunc;
} vUPanelMouseBehavior, *vPUPanelMouseBehavior;

typedef struct vUPanelStyle
{
	vGColor fillColor;
	vGColor borderColor;
	vGColor textColor;
	float   borderWidth;
	float	buttonHoverWidth;
	float	buttonClickWidth;

	vUPanelMouseBehavior mouseBhv;
} vUPanelStyle, *vPUPanelStyle;

typedef struct vUPanel
{
	vUI8 layer;

	vUPanelType panelType;
	vUPanelTextFormat textFormat;
	vPUPanelStyle style;

	vGRect  boundingBox;
	vPGSkin skin;
	vUI8 renderSkin;

	CRITICAL_SECTION textLock;
	vPCHAR text;
	float  textSize;

	vBOOL mouseOver;
	vBOOL mouseClick;
} vUPanel, *vPUPanel;

typedef struct vUserInternals
{
	CRITICAL_SECTION lock;

	vUPanelStyle panelStyleList[MAX_PANEL_STYLES];
	vUI32 panelStyleCount;

	vHNDL panelList;	/* fixed list of all panels */

	vPGSkin defaultTextSkin;

	GLuint panelShaderMesh;
	GLuint panelShaderVertexArray;
	GLuint panelNoSkinTexture;
	vPGShader panelShader;
	vPObject  panelObject;
	vPGRenderable panelRenderer;
} vUserInternals, *vPUserInternals;
vUserInternals _vuser;	/* INSTANCE */


/* ========== CORE FUNCTIONS					==========	*/
VUAPI void vUInitialize(void);

VUAPI vGRect vUCreateRectCenteredOffset(vPosition offset, float width, float height);
VUAPI vGRect vUCreateRectExpanded(vGRect reference, float expansion);

VUAPI vPosition vUScreenToPanelSpace(vPosition screenPos);
VUAPI vPosition vUPanelToScreenSpace(vPosition panelPos);
VUAPI vPosition vUMouseToPanelSpace(void);

VUAPI vBOOL vUIsMouseOverPanel(vPUPanel panel);
VUAPI vBOOL vUIsMouseClickingPanel(vPUPanel panel);

VUAPI vPUPanelStyle vUCreatePanelStyle(vGColor fillColor, vGColor borderColor,
	vGColor textColor, float borderWidth, float buttonHoverWidth, float buttonClickWidth,
	vPUPanelMouseBehavior mouseBehavior);
VUAPI vPUPanel vUCreatePanelRect(vPUPanelStyle style, vGRect rect, vPGSkin imageSkin);
VUAPI vPUPanel vUCreatePanelButton(vPUPanelStyle style, vGRect rect, vPGSkin imageSkin);
VUAPI vPUPanel vUCreatePanelText(vPUPanelStyle style, vGRect rect, vUPanelTextFormat format,
	vPCHAR textPointer);
VUAPI void vUPanelTextLock(vPUPanel panel);
VUAPI void vUPanelTextUnlock(vPUPanel panel);
VUAPI void vUDestroyPanel(vPUPanel panel);

#endif
