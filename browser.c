#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100
typedef char Element;
#include "ArrayStack.h"
#include "CirculerQueue.h"

typedef struct Node {
    char tag_name[10];
    int x, y;
    char hex_color[8];
    char text[20];
} Node;

Node* pnt;

Node* alloc_node(char* str, char* txt) {
    Node* p = (Node*) malloc(sizeof(Node));
    strcpy(p->tag_name, str);
    strcpy(p->text, txt);
    p->x = 30;
    p->y = 30;
    strcpy(p->hex_color, "#000000");
    return p;
}

COLORREF hex_to_color(const char *hex) {
    int r, g, b;
    if(sscanf(hex, "#%02x%02x%02x", &r, &g, &b)==3)
        return RGB(r, g, b);
    return RGB(0, 0, 0);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch(msg) {
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

        SetTextColor(hdc, hex_to_color(pnt->hex_color));
        SetBkMode(hdc, TRANSPARENT);
        TextOutA(hdc, pnt->x, pnt->y, pnt->text, strlen(pnt->text));
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void parse(char* str) {
    char tagName[10] = "";
    char txt[20] = "";
    int i=0;
    int cnt=0;
    while(str[cnt]!='\0') {
        if(str[cnt] == '<' ) {
            push(str[cnt]);
            cnt++;
            continue;
        }
        else if(str[cnt] == '/') {
            if(s_peek()=='<') {
                for(int j=0;j<i;j++) {
                    txt[j] = dequeue();
                }
                init_queue();
                i = 0;
            }
            push(str[cnt]);
            cnt++;
            continue;
        }
        else if(str[cnt] == '>') {
            if(s_peek()=='/') {
                pop();
                pop();
                pop();
                for(int j=0;j<i;j++) {
                    tagName[j] = dequeue();
                }
                i = 0;
                init_queue();
                if(strcmp(tagName, "text")==0) pnt = alloc_node(tagName, txt);
                else if(strcmp(tagName, "box")==0) alloc_node(tagName, txt);
                else if(strcmp(tagName, "link")==0) alloc_node(tagName, txt);
                else if(strcmp(tagName, "title")==0) alloc_node(tagName, txt);
            }
            init_queue();
            i = 0;
            cnt++;
            continue;
        }
        else {
            enqueue(str[cnt]);
            i++;
        }
        cnt++;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    init_stack();
    init_queue();
    char str[100] = "";
    printf("마크업 언어를 작성하세요: \n");
    scanf("%s", str);
    parse(str);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WinProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Hello";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "Hello", "Minimal Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
