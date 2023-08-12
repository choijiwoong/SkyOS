#define SKY_CONSOLE_MODE 0//1이면 콘솔부팅을 의미하며, 아래의 값들은 무시된다.

//세가지 GUI 모드_기본설정 1028x768, 8비트 혹은 32비트 
//1. SkyGUIConsole
#define SKY_GUI_SYSTEM SkyGUIConsole
#define SKY_WIDTH 	1024
#define SKY_HEIGHT 	768
#define SKY_BPP 	32

/*2. SkyGUI
#define SKY_GUI_SYSTEM SkyGUI
#define SKY_WIDTH 	1024
#define SKY_HEIGHT	768
#define SKY_BPP		8 
*/

/*3. SVGA GUI
#define SKY_GUI_SYSTEM SkySVGA
#define SKY_WIDTH	1024
#define SKY_HEIGHT	768
#define SKY_BPP		32
*/
