#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
COORD con_draw_pos;
COORD con_draw_dat;
WORD con_draw_time;
WORD con_draw_attri;
DWORD con_draw_dwWritten;
HANDLE con_draw_stdhandle;
CHAR_INFO *con_draw_buffer;
SYSTEMTIME con_draw_systime;
CONSOLE_CURSOR_INFO con_draw_cci;
CONSOLE_SCREEN_BUFFER_INFO con_draw_csbi;
// 帧高度和帧宽度
SHORT pic_width, pic_height;
// 准备帧
void prepare_picture(int w, int h)
{
	GetLocalTime(&con_draw_systime);
	con_draw_time = con_draw_systime.wMilliseconds;
	pic_width = w;
	pic_height = h;
	if (con_draw_buffer != NULL)
		free(con_draw_buffer);
	con_draw_buffer = (CHAR_INFO *)malloc(w * h * sizeof(CHAR_INFO));
	for (unsigned int i = 0; i < w * h; ++i)
	{
		con_draw_buffer[i].Attributes = con_draw_attri;
		con_draw_buffer[i].Char.AsciiChar = ' ';
	}
}
// 绘制帧
void draw_picture(int x, int y, char ch)
{
	con_draw_buffer[y * pic_width + x].Attributes = con_draw_attri;
	con_draw_buffer[y * pic_width + x].Char.AsciiChar = ch;
}
// 渲染帧
void render_picture(int framerate)
{
	con_draw_pos.X = pic_width;
	con_draw_pos.Y = pic_height;
	con_draw_dat.X = 0;
	con_draw_dat.Y = 0;
	GetConsoleScreenBufferInfo(con_draw_stdhandle, &con_draw_csbi);
	WriteConsoleOutput(con_draw_stdhandle, con_draw_buffer, con_draw_pos, con_draw_dat, &con_draw_csbi.srWindow);
	GetLocalTime(&con_draw_systime);
	Sleep(1000/framerate-(con_draw_systime.wMilliseconds-con_draw_time));
}
// 文字颜色
int fcolor_white = 15, fcolor_black = 0, fcolor_red = 12, fcolor_green = 10, fcolor_blue = 9, fcolor_pink = 13, fcolor_yellow = 14, fcolor_cyan = 11;
// 背景颜色
int bcolor_white = 15, bcolor_black = 8, bcolor_red = 12, bcolor_green = 10, bcolor_blue = 9, bcolor_pink = 13, bcolor_yellow = 14, bcolor_cyan = 11;
// 设置文本颜色
void set_color(int textcolor, int bgcolor)
{
	con_draw_attri = textcolor + (bgcolor - 8) * 16;
}
// 恢复默认颜色
void reset_color()
{
	set_color(15, 8);
}
// 获取终端宽度
int terminal_width()
{
	GetConsoleScreenBufferInfo(con_draw_stdhandle, &con_draw_csbi);
	return con_draw_csbi.srWindow.Right - con_draw_csbi.srWindow.Left;
}
// 获取终端高度
int terminal_height()
{
	GetConsoleScreenBufferInfo(con_draw_stdhandle, &con_draw_csbi);
	return con_draw_csbi.srWindow.Bottom - con_draw_csbi.srWindow.Top;
}
// 设置指针位置
void gotoxy(SHORT x, SHORT y)
{
	con_draw_pos.X = x;
	con_draw_pos.Y = y;
	SetConsoleCursorPosition(con_draw_stdhandle, con_draw_pos);
}
// 设置指针可见
void echo(int mode)
{
	GetConsoleCursorInfo(con_draw_stdhandle, &con_draw_cci);
	con_draw_cci.bVisible = mode;
	SetConsoleCursorInfo(con_draw_stdhandle, &con_draw_cci);
}
// 设置窗口题目
void set_title(const char *title)
{
	SetConsoleTitle((LPCSTR)title);
}
// 清空窗口
void clrscr()
{
	con_draw_pos.X = 0;
	con_draw_pos.Y = 0;
	GetConsoleScreenBufferInfo(
		con_draw_stdhandle,
		&con_draw_csbi);
	FillConsoleOutputAttribute(
		con_draw_stdhandle,
		con_draw_csbi.wAttributes,
		con_draw_csbi.dwSize.X * con_draw_csbi.dwSize.Y,
		con_draw_pos,
		&con_draw_dwWritten);
	FillConsoleOutputCharacterW(
		con_draw_stdhandle,
		L' ',
		con_draw_csbi.dwSize.X * con_draw_csbi.dwSize.Y,
		con_draw_pos,
		&con_draw_dwWritten);
	SetConsoleCursorPosition(
		con_draw_stdhandle,
		con_draw_pos);
}
// 获取键盘输入
int get_kb_input()
{
	return getch();
}
// 判断是否有键按下
int is_kb_hit()
{
	return kbhit();
}
// 加载功能
void init_con_draw()
{
	con_draw_stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	con_draw_buffer = NULL;
	reset_color();
}