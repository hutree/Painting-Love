/*==============================
 Roses.cpp -- һǧ��õ��
  (c) Snow-dream, 2008
  QQ:529698939
  E-mail:xzm2@qq.com
  ��д���ڣ�2008-12-3
  VC 6.0�����±���ͨ��
===============================*/
#include <windows.h>
#include <stdlib.h>
#include "resource.h"
#include <dshow.h>

#define WS_EX_LAYERED 0x00080000
#define LWA_ALPHA     0x00000002
#define LWA_COLORKEY  0x00000001
#define ID_TIMER      1
#define IMGCOUNT      6  //ͼƬ��

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }//�Լ�����һ����

//��Ϊû�е���SetLayeredWindowAttributes API ����Ҫ�����Լ���user32.dll������
typedef BOOL (WINAPI *LPFNSETLAYEREDWINDOWATTRIBUTES)
(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags) ;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int xPosition=600,yPosition=100;
int addPosition=0;
int PicPosition[10][2]={{600,100},{550,80},{500,60},{450,40},{400,20},{350,0},{300,20},{250,40},{200,60}};
//int addFlag=0;
/////��������
IGraphBuilder*   pGBuilder;
IMediaControl*   pMControl; 
IMediaPosition*   pMPos; 

//���ô��ڵ�͸���� hwnd�Ǳ����õĴ��� byAlpha��ָʾ��͸����(0��ȫ͸�� 255��ȫ��͸��)
BOOL TransparentWnd(HWND hwnd, BYTE byAlpha)
{
	long lStyle;
	HMODULE hModule;
	BOOL bRet;
	LPFNSETLAYEREDWINDOWATTRIBUTES lpSetLayeredWindowAttributes;
	if(FALSE == IsWindow(hwnd))
	{
		MessageBox(NULL,TEXT("���ھ����Ч!"),NULL,0);
		return FALSE;
	}
	hModule = (HMODULE)LoadLibrary(TEXT("user32.dll"));
	if(hModule)
	{
		lpSetLayeredWindowAttributes = (LPFNSETLAYEREDWINDOWATTRIBUTES)GetProcAddress(hModule,TEXT("SetLayeredWindowAttributes"));
		if(!lpSetLayeredWindowAttributes)
		{
			MessageBox(hwnd, TEXT("δ�ҵ�API SetLayeredWindowAttributes�ĵ�ַ!"),NULL, MB_OK);
			return FALSE ;
		}
	}
	lStyle = GetWindowLong(hwnd, GWL_EXSTYLE) ;
	if(!lStyle)
	{
		MessageBox(hwnd,TEXT("��ȡ������չ��ʽʧ��!"),NULL,MB_OK);
		return FALSE;
	}
	lStyle = lStyle | WS_EX_LAYERED;
	bRet = SetWindowLong(hwnd,GWL_EXSTYLE,lStyle);
	if(!bRet)
	{
		MessageBox(hwnd,TEXT("���ô�����չ��ʽʧ��!"),NULL,MB_OK);
		return FALSE;
	}
	bRet = lpSetLayeredWindowAttributes(hwnd,0,byAlpha,LWA_ALPHA);
	if(!bRet)
	{
		MessageBox(hwnd,TEXT("���ô���͸����ʧ��!"),NULL,MB_OK);
		return FALSE;
	}
	RedrawWindow(hwnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	FreeLibrary(hModule);
	return TRUE;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	CoInitialize(NULL);   //��ʼ��COM 
	//������������ 
	CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,IID_IGraphBuilder,(void**)&pGBuilder); 
	pGBuilder->QueryInterface(IID_IMediaControl,(void**)&pMControl); 
	pGBuilder->QueryInterface(IID_IMediaPosition,(void**)&pMPos); 
	
	CHAR   strSoundPath[MAX_PATH];   //�洢��������·�� 
	WCHAR   wstrSoundPath[MAX_PATH];   //�洢UNICODE��ʽ��·�� 
	GetCurrentDirectory(MAX_PATH,   strSoundPath); 
	strcat(strSoundPath,"\\������--�Ҿ�֪�����ǰ�.mp3");
	MultiByteToWideChar(CP_ACP,0,strSoundPath,-1,wstrSoundPath,MAX_PATH); 
	pGBuilder->RenderFile(wstrSoundPath,NULL);   //�����ļ� 

	pMPos-> put_CurrentPosition(0);   //�ƶ����ļ�ͷ 
	pMControl->Run();   //���� 
	

	///////////////////////////////////////////
	static TCHAR szAppName[] = TEXT ("Roses");
	HWND     hwnd;
	MSG      msg;
	WNDCLASS wndclass;
	BOOL     bRet;
	
 	wndclass.style          = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc    = WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = hInstance;
	wndclass.hIcon          = LoadIcon (hInstance, szAppName);
	wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground  = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = szAppName;
	
	if(!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	srand ((int)GetTickCount()); //��ʼ�������������
	hwnd = CreateWindow (szAppName, TEXT ("õ�廨"),
		WS_DLGFRAME | WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL, NULL, hInstance, NULL);
    
	TransparentWnd(hwnd, 1); //���ô���͸����
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW); //�����ö�
	ShowWindow (hwnd, iCmdShow);
	UpdateWindow (hwnd);
	
	while ( (bRet = GetMessage (&msg, NULL, 0, 0) ) != 0 )
	{
		if (bRet == -1)
		{
			MessageBox (hwnd, TEXT ("Such code can lead to fatal application errors"),
				szAppName, MB_ICONERROR);
			return -1;
		}
		else
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static      HBITMAP hBmpI[IMGCOUNT], hBmpM[IMGCOUNT] ;
	HINSTANCE   hInstance = NULL ;
	HDC         hdc, hdcMemImag, hdcMemMask ;
	BITMAP      bmp ;
	int i;
	char buf1[100]="ף";
	char buf2[100]="xxx";
	char buf3[100]="xxxxxxxx";
	HDC hdc2=::GetDC(NULL);
	int OldBkMode=::SetBkMode(hdc2,TRANSPARENT);
	HFONT myFont;
	myFont=CreateFont(150,0,0,0,FW_BLACK,FALSE,FALSE,FALSE,   
		GB2312_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,   
		DEFAULT_QUALITY,FIXED_PITCH|FF_MODERN,"����");
	
	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
		for(i=0; i<IMGCOUNT; i++)
		{
			TCHAR szBmpName[16] ;
			wsprintf(szBmpName, TEXT("BM_I%d"), i);
			hBmpI[i] = LoadBitmap (hInstance, szBmpName); //����λͼ
			wsprintf(szBmpName, TEXT("BM_M%d"), i);
			hBmpM[i] = LoadBitmap (hInstance, szBmpName); //������Ӧ������λͼ
		}
		SetTimer(hwnd, ID_TIMER, 100, NULL); //���ö�ʱ��100����
		return 0;
	
		//�������� ���� ����  �˳�����
	case WM_LBUTTONDOWN:
	case WM_KEYDOWN:
		pMControl-> Stop();   //ֹͣ���� 
		//�ͷŶ��� 
		SAFE_RELEASE(pMControl); 
		SAFE_RELEASE(pMPos); 
		SAFE_RELEASE(pGBuilder); 
		CoUninitialize();   //�ͷ�COM 

		SendMessage(hwnd, WM_CLOSE, 0, 0);
		return 0;

	case WM_TIMER: //����Ҫ�Ĳ��� ��ʾ͸��λͼ
		//SelectObject(hdc2,myFont1); //ѡ������1
	    //TextOut(hdc2,50,50,"",strlen(""));
		
		hdc = GetDC(hwnd);
		i = rand()%IMGCOUNT;
		// ����һ����ָ���豸���ݵ��ڴ��豸�����Ļ�����DC��
		hdcMemImag = CreateCompatibleDC (hdc);
		DeleteObject(SelectObject (hdcMemImag, hBmpI[i]));
		GetObject (hBmpI[i], sizeof (BITMAP), &bmp);

		hdcMemMask = CreateCompatibleDC (hdc);
		DeleteObject(SelectObject (hdcMemMask, hBmpM[i]));
		ReleaseDC(hwnd, hdc); //�ͷ��豸�����Ļ�����DC��

		if ((xPosition<=600)&&(xPosition>=200+addPosition)&&(yPosition>=0+addPosition)&&(yPosition<=100+addPosition))
		{	////һ
			xPosition-=10;
			yPosition-=10;
			if (yPosition<0+addPosition)
			{
				yPosition=0+addPosition;
			}
		}
		else if ((xPosition<200+addPosition)&&(xPosition>=100+addPosition)&&(yPosition>=0+addPosition)&&(yPosition<100+addPosition))
		{	////��
			xPosition-=10;
			yPosition+=10;
			if (xPosition<100+addPosition)
			{
				xPosition=100+addPosition;
			}
		}
		else if ((xPosition<=600)&&(xPosition>=100+addPosition)&&(yPosition>=100+addPosition)&&(yPosition<=930-addPosition))
		{	////��
			xPosition+=10;
			yPosition+=10;
		}
		else if ((xPosition<=1100-addPosition)&&(xPosition>=600)&&(yPosition>=100+addPosition)&&(yPosition<=930-addPosition))
		{	////��
			xPosition+=10;
			yPosition-=10;
			if (xPosition>1100-addPosition)
			{
				xPosition=1100-addPosition;
			}
		}
		else if ((xPosition<=1100-addPosition)&&(xPosition>1000-addPosition)&&(yPosition>=0+addPosition)&&(yPosition<=100+addPosition))
		{	////��
			xPosition-=10;
			yPosition-=10;
			if (yPosition<0+addPosition)
			{
				yPosition=0+addPosition;
			}
		}
		else if ((xPosition<=1000-addPosition)&&(xPosition>600)&&(yPosition>=0+addPosition)&&(yPosition<=100+addPosition))
		{	////��
			xPosition-=10;
			if (xPosition<=700)
			{
				yPosition+=10;
			}
			if (yPosition>=100+addPosition)
			{
				yPosition=99+addPosition;
			}
			if (xPosition<=600)
			{
				addPosition+=70;
			}
			if (addPosition==280)
			{
				xPosition = 1100;
				yPosition = 9000;
			}
		} 
		else
		{
			SelectObject(hdc2,myFont); //ѡ������
			SetTextColor(hdc2,RGB(255,64,64)); 
			TextOut(hdc2,630,400,buf1,strlen(buf1));
			Sleep(2000);
			SetTextColor(hdc2,RGB(138,43,226)); 
			TextOut(hdc2,500,250,buf2,strlen(buf2));
			Sleep(2000);
			SetTextColor(hdc2,RGB(255,0,255)); 
			TextOut(hdc2,180,100,buf3,strlen(buf3));
			ReleaseDC(NULL,hdc2);
		}

		hdc= GetDC(NULL);
		//SetBkMode(hdc,TRANSPARENT);
		//����λͼ
// 		if (addPosition<=10)
// 		{
// 			BitBlt(hdc,PicPosition[addPosition][0],PicPosition[addPosition][1],bmp.bmWidth,bmp.bmHeight,hdcMemMask,0,0,SRCAND);
// 			BitBlt(hdc,PicPosition[addPosition][0],PicPosition[addPosition][1],bmp.bmWidth,bmp.bmHeight,hdcMemImag,0,0,SRCPAINT);
// 		}
//		addPosition++;

		BitBlt(hdc,xPosition,yPosition,bmp.bmWidth,bmp.bmHeight,hdcMemMask,0,0,SRCAND);
		BitBlt(hdc,xPosition,yPosition,bmp.bmWidth,bmp.bmHeight,hdcMemImag,0,0,SRCPAINT);

		//CBitmap FootballBMP;
		//FootballBMP.LoadBitmap(IDB_FOOTBALLBMP);
// 		CDC ImageDC;
// 		ImageDC.CreateCompatibleDC(hdc);
// 		CBitmap *pOldImageBMP = ImageDC.SelectObject(&hdcMemImag);
// 		TransparentBlt(hdc,0,0,218,199,ImageDC.m_hDC,0,0,218,199,RGB(0,0,0xff));
// 		ImageDC.SelectObject(pOldImageBMP);
		//ɾ��ָ�����豸�����Ļ�����DC��
		DeleteDC (hdcMemMask);
		DeleteDC (hdcMemImag);
		ReleaseDC(NULL,hdc);
		return 0 ;
		
	case WM_DESTROY:
		// ɾ����ʱ��
		KillTimer(hwnd,ID_TIMER);
		// ɾ��λͼ������λͼ
		for(i=0;i<IMGCOUNT;i++)
		{
			DeleteObject (hBmpI[i]);
			DeleteObject (hBmpM[i]);
		}
		InvalidateRect(NULL, NULL, TRUE); //����������Ļ
		PostQuitMessage (0);
		return 0;
	}
	return DefWindowProc (hwnd, message, wParam, lParam);
}