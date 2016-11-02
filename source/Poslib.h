// POSLIB.h : main header file for the POSLIB application
//

#if !defined(AFX_POSLIB_H__B57BA06C_2C04_4842_9B90_5DB9A5B58F3B__INCLUDED_)
#define AFX_POSLIB_H__B57BA06C_2C04_4842_9B90_5DB9A5B58F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


class CPOSLIBApp : public CWinApp
{
public:
	CPOSLIBApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOSLIBApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CPOSLIBApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSLIB_H__B57BA06C_2C04_4842_9B90_5DB9A5B58F3B__INCLUDED_)



