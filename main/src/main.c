
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "lv_drv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
//#include "lvgl/demos/lv_demos.h"
#if USE_SDL
  #define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
  #include <SDL2/SDL.h>
  #include "lv_drivers/sdl/sdl.h"
#elif USE_X11
  #include "lv_drivers/x11/x11.h"
#endif
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mousewheel.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static void hal_deinit(void);
static void* tick_thread(void *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static pthread_t thr_tick;    /* thread */
static bool end_tick = false; /* flag to terminate thread */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if 0
static void user_image_demo()
{
  lv_obj_t * img = lv_gif_create(lv_scr_act());
  lv_gif_set_src(img, "A:lvgl/examples/libs/gif/bulb.gif");
  lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

  lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(lv_scr_act(), 150, fg_color, bg_color);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);

    /*Create a font*/
    static lv_ft_info_t info;
    /*FreeType uses C standard file system, so no driver letter is required.*/
    info.name = "./lvgl/examples/libs/freetype/Lato-Regular.ttf";
    info.weight = 24;
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    if(!lv_ft_font_init(&info)) {
        LV_LOG_ERROR("create failed.");
    }

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font created with FreeType");
    lv_obj_set_pos(label, 10, 10);

    lv_obj_t *  img1 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img1, "A:lvgl/examples/libs/png/wink.png");
    lv_obj_align(img1, LV_ALIGN_LEFT_MID, 20, 0);

    lv_obj_t * wp;

    wp = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(wp, "A:lvgl/examples/libs/sjpg/small_image.sjpg");
    lv_obj_align(wp, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_t * img2 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img2, "A:lvgl/examples/libs/sjpg/lv_example_jpg.jpg");
    //lv_obj_center(img);
    lv_obj_align(img2, LV_ALIGN_TOP_RIGHT, -20, 20);

    lv_obj_t * img3 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
#if LV_COLOR_DEPTH == 32
    lv_img_set_src(img3, "A:lvgl/examples/libs/bmp/example_32bit.bmp");
#elif LV_COLOR_DEPTH == 16
    lv_img_set_src(img, "A:lvgl/examples/libs/bmp/example_16bit.bmp");
#endif
    lv_obj_align(img3, LV_ALIGN_BOTTOM_MID, 0, -20);

    lv_obj_t * img4 = lv_img_create(lv_scr_act());
    lv_img_set_src(img4, "A:lvgl/examples/libs/ffmpeg/ffmpeg.png");
    lv_obj_align(img4, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    lv_obj_t * player = lv_ffmpeg_player_create(lv_scr_act());
    lv_ffmpeg_player_set_src(player, "./lvgl/examples/libs/ffmpeg/birds.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_align(player, LV_ALIGN_TOP_MID, 0, 20);
}
#endif

#include <time.h>
#include <math.h>
#define WIN_WIDTH  LV_HOR_RES
#define WIN_HEIGHT LV_VER_RES

// 定义表盘半径和中心点
#define CLOCK_RADIUS 110
#define CLOCK_CENTER_X 100
#define CLOCK_CENTER_Y 130

// 定义指针长度
#define HOUR_HAND_LENGTH   70
#define MINUTE_HAND_LENGTH 80
#define SECOND_HAND_LENGTH 90

// 定义指针颜色
#define HOUR_HAND_COLOR   lv_color_hex(0x0000FF)   // 蓝色
#define MINUTE_HAND_COLOR lv_color_hex(0x00FF00)   // 绿色
#define SECOND_HAND_COLOR lv_color_hex(0xFF0000)   // 红色

//数字显示文本的长度和字体和颜色
#define LABEL_COLOR   lv_color_hex(0x000000)
#define LABEL_FONT    lv_font_montserrat_18
#define LABEL_LENGTH  CLOCK_RADIUS-12

// 全局变量：指针对象
static lv_obj_t* hour_hand;
static lv_obj_t* minute_hand;
static lv_obj_t* second_hand;

// 定义变量存储选中的数字
static int clock_hour = 0;
static int clock_minute = 0;
static int clock_second = 0;

// 定义滚动表盘显示的数字
static lv_obj_t* roller_hour;
static lv_obj_t* roller_minute;
static lv_obj_t* roller_second;

static bool buffer_set_time = false;




// 定义窗口数量
#define NUM_WINDOWS 3

// 全局变量：窗口对象和当前窗口索引
static lv_obj_t* windows[NUM_WINDOWS];
static int current_window = 0;




// 创建窗口函数
lv_obj_t* create_window(lv_obj_t* parent, const char* text, lv_color_t bg_color) {
    lv_obj_t* win = lv_obj_create(parent);
    lv_obj_set_size(win, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(win, bg_color, 0); // 设置背景颜色

    // 创建标签
    lv_obj_t* label = lv_label_create(win);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    return win;
}



//滑动动画回调函数
static void set_x_anim(void* obj, int32_t value) {
    lv_obj_set_x((lv_obj_t*)obj, value);
}

static void switch_window(int direction) {
    int target_window = current_window + direction;

    // 处理边界
    if (target_window < 0) {
        target_window = NUM_WINDOWS - 1;
    } else if (target_window >= NUM_WINDOWS) {
        target_window = 0;
    }

    // 计算目标窗口的 X 坐标
    int target_x = direction * LV_HOR_RES;

    // 动画：当前窗口向左或向右移出
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, windows[current_window]);
    lv_anim_set_values(&a, lv_obj_get_x(windows[current_window]), target_x);
    lv_anim_set_exec_cb(&a, set_x_anim);
    lv_anim_set_time(&a, 300);
    lv_anim_start(&a);

    // 动画：目标窗口从左侧或右侧进入
    lv_anim_set_var(&a, windows[target_window]);
    lv_anim_set_values(&a, -target_x, 0);
    lv_anim_start(&a);

    // 更新当前窗口索引
    current_window = target_window;
}




// 手势事件处理函数
static void event_handler(lv_event_t* e) {
    lv_obj_t* scr = lv_event_get_target(e); // 获取事件目标（屏幕）
    lv_indev_t* indev = lv_indev_get_act(); // 获取当前输入设备
    lv_point_t point;
    lv_indev_get_vect(indev, &point); // 获取滑动向量

    if (e->code == LV_EVENT_GESTURE) {
        if (point.x < -50) { // 向左滑动
            switch_window(1); // 切换到下一个窗口
        } else if (point.x > 50) { // 向右滑动
            switch_window(-1); // 切换到上一个窗口
        }
    }
}


    // 定义直线的起点和终点坐标
    static lv_point_t line_points_hour[] = {
        {105, 125},  // 起点 (x1, y1)
        {200, 100} // 终点 (x2, y2)
    };

        // 定义直线的起点和终点坐标
    static lv_point_t line_points_min[] = {
        {105, 125},  // 起点 (x1, y1)
        {150, 50} // 终点 (x2, y2)
    };

        // 定义直线的起点和终点坐标
    static lv_point_t line_points_sec[] = {
        {105, 125},  // 起点 (x1, y1)
        {40, 100} // 终点 (x2, y2)
    };


// 计算指针端点坐标
static void calculate_hand_position(double angle, int length, lv_point_t* point) {
    double radians = angle * M_PI / 180.0; // 角度转弧度
    point->x = 105 + (int)(length * sin(radians));
    point->y = 125 - (int)(length * cos(radians));
}

// 更新指针位置
static void update_clock_hands(int hour, int min, int sec) {

    // 计算角度
    double hour_angle = (hour % 12) * 30.0 + min * 0.5;   // 每小时 30 度，每分钟 0.5 度
    double min_angle = min * 6.0;                         // 每分钟 6 度
    double sec_angle = sec * 6.0;                         // 每秒 6 度

    // 计算指针端点坐标
    lv_point_t hour_point, min_point, sec_point;
    calculate_hand_position(hour_angle, HOUR_HAND_LENGTH, &hour_point);
    calculate_hand_position(min_angle, MINUTE_HAND_LENGTH, &min_point);
    calculate_hand_position(sec_angle, SECOND_HAND_LENGTH, &sec_point);

    line_points_hour[1] = hour_point;
    line_points_min[1] = min_point;
    line_points_sec[1] = sec_point;

    lv_line_set_points(hour_hand, line_points_hour, 2);
    lv_line_set_points(minute_hand, line_points_min, 2);
    lv_line_set_points(second_hand, line_points_sec, 2);

}

// 定时器回调函数
static void timer_callback(lv_timer_t* timer) {
    // 获取当前时间（模拟 RTC 读取）
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);

    // 更新指针位置
    update_clock_hands(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    if(buffer_set_time == false)
    {
      lv_roller_set_selected(roller_hour,timeinfo->tm_hour,LV_ANIM_OFF);
      lv_roller_set_selected(roller_minute,timeinfo->tm_min,LV_ANIM_OFF);
    }

    //update_clock_hands(5, 4, 30);
}

// 滚盘事件回调函数
static void roller_event_handler(lv_event_t* e) {
    lv_obj_t* roller = lv_event_get_target(e); // 获取触发事件的滚盘对象
    uint16_t selected = lv_roller_get_selected(roller); // 获取选中的索引
    buffer_set_time = true;
    // 根据滚盘的 ID 更新对应的变量
    if (lv_obj_get_index(roller) == 0) {
        clock_hour = selected;
    } else if (lv_obj_get_index(roller) == 1) {
        clock_minute = selected;
    } else if (lv_obj_get_index(roller) == 2) {
        clock_second = selected;
    } else if (lv_obj_get_index(roller) == 3) {

    }
    update_clock_hands(clock_hour,clock_minute,clock_second);
    buffer_set_time = false;
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1111(void)
{


// 获取默认屏幕
    lv_obj_t* scr = lv_scr_act();

    // 创建三个窗口
    // windows[0] = create_window(scr, "Window 1", lv_color_hex(0xFFAAAA)); // 浅红色背景
    // windows[1] = create_window(scr, "Window 2", lv_color_hex(0xAAFFAA)); // 浅绿色背景
    // windows[2] = create_window(scr, "Window 3", lv_color_hex(0xAAAAFF)); // 浅蓝色背景



    lv_obj_t* win1 = lv_obj_create(scr);
    lv_obj_set_size(win1, WIN_WIDTH, WIN_HEIGHT);
    lv_obj_set_style_bg_color(win1, lv_color_hex(0xFFFFFF), 0); // 设置背景颜色
    lv_obj_set_x(win1, 0); // 初始位置在屏幕左侧

    lv_obj_t* win2 = lv_obj_create(scr);
    lv_obj_set_size(win2, WIN_WIDTH, WIN_HEIGHT);
    lv_obj_set_style_bg_color(win2, lv_color_hex(0xFFFFFF), 0); // 设置背景颜色
    lv_obj_set_x(win2, WIN_WIDTH); // 初始位置在屏幕右侧

    lv_obj_t* win3 = lv_obj_create(scr);
    lv_obj_set_size(win3, WIN_WIDTH, WIN_HEIGHT);
    lv_obj_set_style_bg_color(win3, lv_color_hex(0xFFFFFF), 0); // 设置背景颜色
    lv_obj_set_x(win3, WIN_WIDTH); // 初始位置在屏幕右侧

    windows[0] = win1;
    windows[1] = win2;
    windows[2] = win3;

     // 创建小时滚盘
    roller_hour = lv_roller_create(win2);
    lv_roller_set_options(roller_hour, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_hour, 50, 150);
    lv_obj_align(roller_hour, LV_ALIGN_CENTER, -70, -0);
    lv_obj_add_event_cb(roller_hour, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建分钟滚盘
    roller_minute = lv_roller_create(win2);
    lv_roller_set_options(roller_minute, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_minute, 50, 150);
    lv_obj_align(roller_minute, LV_ALIGN_CENTER, 0, -0);
    lv_obj_add_event_cb(roller_minute, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建秒滚盘
    roller_second = lv_roller_create(win2);
    lv_roller_set_options(roller_second, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_second, 50, 150);
    lv_obj_align(roller_second, LV_ALIGN_CENTER, 70, -0);
    lv_obj_add_event_cb(roller_second, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

     // 创建圆形钟表背景
    lv_obj_t * clock = lv_obj_create(win1);

    // 设置圆形的宽度和高度相等，确保它是一个圆形
    lv_obj_set_size(clock, CLOCK_RADIUS*2, CLOCK_RADIUS*2);  // 250x250 像素的钟表背景

    // 设置钟表的位置
    lv_obj_align(clock, LV_ALIGN_CENTER, 0, 0);  // 将钟表置于屏幕中心

    // 设置钟表背景样式
    static lv_style_t style;
    lv_style_init(&style);

    // 设置背景颜色为灰色
    lv_style_set_bg_color(&style, lv_color_hex(0x808080));

    // 设置圆形的圆角为半径大小，使其成为圆形
    lv_style_set_radius(&style, LV_RADIUS_CIRCLE);

    // 应用样式到钟表对象
    lv_obj_add_style(clock, &style, 0);

    // 创建一条直线
    hour_hand = lv_line_create(win1);
    // 设置直线的点坐标
    lv_line_set_points(hour_hand, line_points_hour, 2);
    // 设置直线的样式
    lv_obj_set_style_line_width(hour_hand, 2, 0);              // 线宽为 2
    lv_obj_set_style_line_color(hour_hand, lv_color_hex(0xFF0000), 0); // 红色
    lv_obj_set_style_line_opa(hour_hand, LV_OPA_COVER, 0);     // 完全不透明

    // 创建一条直线
    minute_hand = lv_line_create(win1);
    // 设置直线的点坐标
    lv_line_set_points(minute_hand, line_points_min, 2);
    // 设置直线的样式
    lv_obj_set_style_line_width(minute_hand, 3, 0);              // 线宽为 2
    lv_obj_set_style_line_color(minute_hand, lv_color_hex(0x00FF00), 0); // 红色
    lv_obj_set_style_line_opa(minute_hand, LV_OPA_COVER, 0);     // 完全不透明

    // 创建一条直线
    second_hand = lv_line_create(win1);
    // 设置直线的点坐标
    lv_line_set_points(second_hand, line_points_sec, 2);
    // 设置直线的样式
    lv_obj_set_style_line_width(second_hand, 4, 0);              // 线宽为 2
    lv_obj_set_style_line_color(second_hand, lv_color_hex(0x00FFFF), 0); // 红色
    lv_obj_set_style_line_opa(second_hand, LV_OPA_COVER, 0);     // 完全不透明

    // 创建标签并设置文本
    lv_obj_t* label12 = lv_label_create(win1);
    lv_label_set_text(label12, "12"); // 设置文本内容
    lv_obj_set_style_text_color(label12, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label12, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label12, LV_ALIGN_CENTER, 0, -(LABEL_LENGTH)); // 位置显示

    lv_obj_t* label3 = lv_label_create(win1);
    lv_label_set_text(label3, "3"); // 设置文本内容
    lv_obj_set_style_text_color(label3, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label3, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label3, LV_ALIGN_CENTER, LABEL_LENGTH, 0); // 位置显示

    lv_obj_t* label6 = lv_label_create(win1);
    lv_label_set_text(label6, "6"); // 设置文本内容
    lv_obj_set_style_text_color(label6, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label6, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label6, LV_ALIGN_CENTER, 0, LABEL_LENGTH); // 位置显示

    lv_obj_t* label9 = lv_label_create(win1);
    lv_label_set_text(label9, "9"); // 设置文本内容
    lv_obj_set_style_text_color(label9, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label9, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label9, LV_ALIGN_CENTER, -(LABEL_LENGTH), 0); // 位置显示

    lv_obj_t* label1 = lv_label_create(win1);
    lv_label_set_text(label1, "1"); // 设置文本内容
    lv_obj_set_style_text_color(label1, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label1, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label1, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(30.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(30.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label2 = lv_label_create(win1);
    lv_label_set_text(label2, "2"); // 设置文本内容
    lv_obj_set_style_text_color(label2, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label2, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label2, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(60.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(60.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label4 = lv_label_create(win1);
    lv_label_set_text(label4, "4"); // 设置文本内容
    lv_obj_set_style_text_color(label4, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label4, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label4, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(120.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(120.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label5 = lv_label_create(win1);
    lv_label_set_text(label5, "5"); // 设置文本内容
    lv_obj_set_style_text_color(label5, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label5, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label5, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(150.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(150.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label7 = lv_label_create(win1);
    lv_label_set_text(label7, "7"); // 设置文本内容
    lv_obj_set_style_text_color(label7, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label7, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label7, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(210.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(210.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label8 = lv_label_create(win1);
    lv_label_set_text(label8, "8"); // 设置文本内容
    lv_obj_set_style_text_color(label8, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label8, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label8, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(240.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(240.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label10 = lv_label_create(win1);
    lv_label_set_text(label10, "10"); // 设置文本内容
    lv_obj_set_style_text_color(label10, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label10, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label10, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(300.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(300.0 * M_PI / 180.0)); // 位置显示

    lv_obj_t* label11 = lv_label_create(win1);
    lv_label_set_text(label11, "11"); // 设置文本内容
    lv_obj_set_style_text_color(label11, LABEL_COLOR, 0); // 文本颜色
    lv_obj_set_style_text_font(label11, &LABEL_FONT, 0); // 字体大小
    lv_obj_align(label11, LV_ALIGN_CENTER, (LABEL_LENGTH)*sin(330.0 * M_PI / 180.0), -(LABEL_LENGTH)*cos(330.0 * M_PI / 180.0)); // 位置显示


    // 初始化窗口位置
    lv_obj_set_x(windows[0], 0); // 第一个窗口在屏幕左侧
    lv_obj_set_x(windows[1], LV_HOR_RES); // 第二个窗口在屏幕右侧
    lv_obj_set_x(windows[2], 2*LV_HOR_RES); // 第三个窗口在屏幕右侧

    // 添加手势事件处理
    lv_obj_add_event_cb(scr, event_handler, LV_EVENT_GESTURE, NULL);

    update_clock_hands(11,30,45);
    // 创建定时器，每 100ms 更新一次
    lv_timer_create(timer_callback, 100, NULL);


  // 创建第一个窗口
  //lv_obj_t* win1 = create_window(scr, "Window 1", "Label 1", lv_color_hex(0xFFFFFF)); // 浅红色背景
  // 创建表盘背景




  // lv_obj_t* win1 = lv_obj_create(scr);
  // lv_obj_set_size(win1, WIN_WIDTH, WIN_HEIGHT);
  // lv_obj_set_style_bg_color(win1, lv_color_hex(0xFFFFFF), 0); // 设置背景颜色

  //lv_obj_set_x(clock_bg, 0); // 初始位置在屏幕左侧

  // 创建第二个窗口
  //lv_obj_t* win2 = create_window(scr, "Window 2", "Label 2", lv_color_hex(0xFFFFFF)); // 浅绿色背景
  //lv_obj_set_x(win2, WIN_WIDTH); // 初始位置在屏幕右侧



  //update_clock_hands(5, 4, 3);


}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();

lv_example_get_started_1111();
//lv_example_switch_1();
//  lv_example_calendar_1();
//  lv_example_btnmatrix_2();
//  lv_example_checkbox_1();
//  lv_example_colorwheel_1();
//  lv_example_chart_6();
//  lv_example_table_2();
//  lv_example_scroll_2();
//  lv_example_textarea_1();
//  lv_example_msgbox_1();
//  lv_example_dropdown_2();
//  lv_example_btn_1();
//  lv_example_scroll_1();
//  lv_example_tabview_1();
//  lv_example_tabview_1();
//  lv_example_flex_3();
//  lv_example_label_1();

//  lv_demo_widgets();
//  lv_demo_keypad_encoder();
//  lv_demo_benchmark();
//  lv_demo_stress();
//  lv_demo_music();

//  user_image_demo();

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    // 创建定时器，每 100ms 更新一次
    //lv_timer_create(timer_callback, 100, NULL);
    lv_timer_handler();
    usleep(5 * 1000);
  }

  hal_deinit();
  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* mouse input device */
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1);
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /* keyboard input device */
  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2);
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

  /* mouse scroll wheel input device */
  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3);
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

  lv_disp_t *disp = NULL;

#if USE_SDL
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  sdl_init();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
  static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  // mouse_init();
  indev_drv_1.read_cb = sdl_mouse_read;

  // keyboard_init();
  indev_drv_2.read_cb = sdl_keyboard_read;

  // mousewheel_init();
  indev_drv_3.read_cb = sdl_mousewheel_read;

#elif USE_X11
  lv_x11_init("LVGL Simulator Demo", DISP_HOR_RES, DISP_VER_RES);

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[DISP_HOR_RES * 100];
  static lv_color_t buf1_2[DISP_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = lv_x11_flush;
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  indev_drv_1.read_cb = lv_x11_get_pointer;
  indev_drv_2.read_cb = lv_x11_get_keyboard;
  indev_drv_3.read_cb = lv_x11_get_mousewheel;
#endif
  /* Set diplay theme */
  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  /* Tick init */
  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);

  /* register input devices */
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(kb_indev, g);
  lv_indev_set_group(enc_indev, g);

  /* Set a cursor for the mouse */
  LV_IMG_DECLARE(mouse_cursor_icon);                   /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor*/
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);      /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);        /*Connect the image  object to the driver*/
}

/**
 * Releases the Hardware Abstraction Layer (HAL) for the LVGL graphics library
 */
static void hal_deinit(void)
{
  end_tick = true;
  pthread_join(thr_tick, NULL);

#if USE_SDL
  // nop
#elif USE_X11
  lv_x11_deinit();
#endif
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static void* tick_thread(void *data) {
  (void)data;

  while(!end_tick) {
    usleep(5000);
    lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
  }

  return NULL;
}
