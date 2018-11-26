#include "con_draw.h"
int main()
{
    init_con_draw();
    int x=0, xi=1, y=0, yi=1;
    set_color(fcolor_blue, bcolor_white);
    for(;;)
    {
        if(is_kb_hit())
        {
            get_kb_input();
            echo(1);
            return 0;
        }
        prepare_picture(terminal_width(), terminal_height());
        draw_picture(x, y, '@');
        x=x+xi;
        y=y+yi;
        if(x==0)
            xi=1;
        if(x==pic_width-1)
            xi=-1;
        if(y==0)
            yi=1;
        if(y==pic_height-1)
            yi=-1;
        render_picture(30);
        echo(0);
    }
}