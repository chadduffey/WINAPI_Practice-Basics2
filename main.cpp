/* 
 * author/id:	Chad Duffey 
 * purpose: Learning to use shapes and mouse events
 * date created: 07/08/2011
 * date last Modified: 07/08/2011
 */

#include <windows.h>
#include <string.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "Assignment1-Q2";

/* Program Functions */

//updates the status bar with the type of drawing being done
void updateStatusBar(HWND hwnd, int drawType);

//update the user on which color they are using
void messageBoxDialog(int colour);

struct myObject
{
	int type; //0=rect, 1=ellipse
	POINT ptBeg, ptEnd;
	int brush;
	// myObject * pNext; LL-version
};

const int MAX_OBJECTS = 50;
myObject objectArray[MAX_OBJECTS];
int objectCount = 0;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;				/* This is the handle for our window */
    MSG messages;			/* Here messages to the application are saved */
    WNDCLASSEX wincl;		/* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WndProc;  /* Message handler */
    wincl.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;   /* No menu */
    wincl.cbClsExtra = 0;        
    wincl.cbWndExtra = 0;  
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

	//objectArray = new myObject[50];

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   
           szClassName,           /* Classname */
           "Assignment Solution", /* Title Text */
           WS_OVERLAPPEDWINDOW,   /* default window */
           CW_USEDEFAULT,         /* Windows position */
           CW_USEDEFAULT,         /* on the screen */
           544,                   /* Width */
           375,                   /* Height, in pixels */
           HWND_DESKTOP,          /* Parent window */
           NULL,                  /* No menu */
           hThisInstance,         /* Program Instance handler */
           NULL                   /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    return messages.wParam;
}


void DrawBoxOutline (HWND hwnd, POINT ptBeg, POINT ptEnd)
{
	HDC hdc;

	hdc = GetDC (hwnd);

	SetROP2(hdc, R2_NOT);
	SelectObject(hdc, GetStockObject (NULL_BRUSH));
	Rectangle (hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);

	ReleaseDC (hwnd, hdc);
}




/*  This function is called by the function DispatchMessage() */
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL fBlocking, fValidBox, currentShape;
	static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
	static int brushToUse; 

	int clientWidth, clientHeight;
	
	HDC hdc;				// Handle to a device context, for drawing
    PAINTSTRUCT ps;			// Extra information useful when painting
        
    switch (message)        /* handle the messages */
    {
        //-------------------------------------------
        case WM_CREATE:
            // Once only initialisation
			
			//set the shape to be drawn as rectangle to start with
			//rectangle = true (0), ellipse = false (1);
			
			currentShape = TRUE;		//rectangle objects are default (right click to change)
			brushToUse = 2;				//set to grey to start with (double right click to change)
			updateStatusBar(hwnd, 1);	//update the status bar to suit the drawing
		
			return 0;

        //-------------------------------------------
		case WM_LBUTTONDOWN:
			ptBeg.x = ptEnd.x = LOWORD(lParam);
			ptBeg.y = ptEnd.y = HIWORD(lParam);

			DrawBoxOutline (hwnd, ptBeg, ptEnd);

			SetCapture (hwnd);
			SetCursor (LoadCursor (NULL, IDC_CROSS));

			fBlocking = TRUE;
			return 0;

        //-------------------------------------------        
		case WM_RBUTTONDOWN:
			if (currentShape)
			{
				currentShape = FALSE;
				updateStatusBar(hwnd, 2);
			}
			else
			{
				currentShape = TRUE;
				updateStatusBar(hwnd, 1);
			}
			
			return 0;
		//-------------------------------------------  
		case WM_MOUSEMOVE:
			if (fBlocking)
			{
				SetCursor (LoadCursor (NULL, IDC_CROSS));

				DrawBoxOutline(hwnd, ptBeg, ptEnd);

				ptEnd.x = LOWORD (lParam);
				ptEnd.y = HIWORD (lParam);

				DrawBoxOutline (hwnd, ptBeg, ptEnd);
			}
			return 0;

		//-------------------------------------------  
		case WM_LBUTTONUP:
			if (fBlocking)
			{
				DrawBoxOutline (hwnd, ptBeg, ptEnd);

				ReleaseCapture();

				SetCursor (LoadCursor (NULL, IDC_ARROW));

				fBlocking = FALSE;
				fValidBox = TRUE;
				ptBoxBeg = ptBeg;
				ptBoxEnd.x = LOWORD (lParam);
				ptBoxEnd.y = HIWORD (lParam);

				if (abs(ptBoxBeg.x - ptBoxEnd.x) < 3 && abs(ptBoxBeg.y - ptBoxEnd.y) < 3)
					break;
				
				if (objectCount < MAX_OBJECTS - 1)
				{
					objectArray[objectCount].brush = brushToUse;
					objectArray[objectCount].ptBeg= ptBeg;
					objectArray[objectCount].ptEnd= ptEnd;
					objectArray[objectCount].type = (currentShape==true)?0:1;
					objectCount++;
				}
				InvalidateRect (hwnd, NULL, FALSE);
			}


			return 0;
		//-------------------------------------------  
		case WM_CHAR:
			if (wParam == '\x1B') //escape key
			{
				//clear the work area
				if (objectCount > 1)
				{
					objectArray[0] = objectArray[objectCount -1];
					objectCount = 1;
				}

				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		//-------------------------------------------  
		case WM_LBUTTONDBLCLK:
			
			//clear the work area
			if (objectCount > 1)
			{
				objectArray[0] = objectArray[objectCount -1];
				objectCount = 1;
			}

			InvalidateRect(hwnd, NULL, TRUE);
						
			return 0;
		//-------------------------------------------  
		case WM_RBUTTONDBLCLK:
			//alternates between brushes
			if (brushToUse < 5)
			{	
				brushToUse++;
				messageBoxDialog(brushToUse);
			}
			else
			{
				brushToUse = 0;
				messageBoxDialog(brushToUse);
			}
				
			return 0;
		//-------------------------------------------
		case WM_PAINT:
             hdc = BeginPaint( hwnd, &ps );
             
             if (fValidBox)
			 {
				 for (int i = 0; i < objectCount; i++)
				 {
					 SelectObject (hdc, GetStockObject (objectArray[i].brush));
					 switch(objectArray[i].type)
					 {
					 case 0:
						 Rectangle (hdc, objectArray[i].ptBeg.x, objectArray[i].ptBeg.y, objectArray[i].ptEnd.x, objectArray[i].ptEnd.y);
						 break;
					 case 1:
						 Ellipse(hdc, objectArray[i].ptBeg.x, objectArray[i].ptBeg.y, objectArray[i].ptEnd.x, objectArray[i].ptEnd.y);
					 }
				 }
				 /*
				 SelectObject (hdc, GetStockObject (brushToUse));
				 if (currentShape)
				 //rectangle is selected
				 {	
					 Rectangle (hdc, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
				 }
				 else
				//ellipse is selected
				 {
					 Ellipse(hdc, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
				 }
				 */
			 }

			 if (fBlocking)
			 {
				 SetROP2 (hdc, R2_NOT);
				 SelectObject (hdc, GetStockObject (NULL_BRUSH));
				 Rectangle (hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
			 }

             EndPaint( hwnd, &ps );
			 return 0;
             
       //-------------------------------------------
        case WM_DESTROY:
             // Once only cleanup.
            PostQuitMessage (0);       
			/* WM_QUIT  */
        return 0;
        
        default:
			/* for messages that we don't deal with */
			return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void updateStatusBar(HWND hwnd, int drawType)
//updates the status bar with the type of drawing being done
{
	if (drawType == 1)
	{
		LPCTSTR strMsg = "Current Shape: Rectangle";
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)strMsg);
	}
	if (drawType == 2)
	{
		LPCTSTR strMsg = "Current Shape: Ellipse";
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)strMsg);
	}
}

void messageBoxDialog(int colour)
{
	/*
	#define WHITE_BRUSH         0
	#define LTGRAY_BRUSH        1
	#define GRAY_BRUSH          2
	#define DKGRAY_BRUSH        3
	#define BLACK_BRUSH         4
	#define NULL_BRUSH          5
	*/
	
	switch(colour)
	{	
		case 0:
			MessageBox(NULL, "Switching to the WHITE Brush", "Brush Change", NULL);
			break;
		case 1:
			MessageBox(NULL, "Switching to the LIGHT GREY Brush", "Brush Change", NULL);
			break;
		case 2:
			MessageBox(NULL, "Switching to the GREY Brush Brush", "Brush Change", NULL);
			break;
		case 3:
			MessageBox(NULL, "Switching to the DARK GREY Brush", "Brush Change", NULL);
			break;
		case 4:
			MessageBox(NULL, "Switching to the BLACK Brush", "Brush Change", NULL);
			break;
		case 5:
			MessageBox(NULL, "Switching to the CLEAR Brush", "Brush Change", NULL);
			break;
		default:
			MessageBox(NULL, "Switching to the {error} Brush", "Brush Change", NULL);
	}
}
