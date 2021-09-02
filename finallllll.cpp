#include<iostream>
#include<windows.h>
#include<string>
#include<cstdlib>
#include<conio.h>
#include <stdio.h>
#include<ctime>
#include<fstream>

 
/* 俄羅斯方塊  Tetris */

using namespace std;


/* 工具箱  */
HANDLE hIn,hOut;
//取得後臺處理I/O控制權，hIn=>鍵盤控制  hOut=>螢幕輸出控制
void Control_HANDLE_Unit()
{
hIn=GetStdHandle(STD_INPUT_HANDLE);
hOut=GetStdHandle(STD_OUTPUT_HANDLE);
}
//座標給定函式
void position(int x,int y)
{
	static COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(hOut,c);
}

/*遊戲主體*/
/** 遊戲框架 及 初始化 **/
const int nWidth=16,nHeight=21;
const int nextWidth=7,nextHeight=7;
char Unit[4]={'|','=','+','O'};//邊、底邊、方塊
int frame[nHeight][nWidth]={0}; // 後臺參數，框架中frame[y][x] x,y為x座標和y座標; 其值=>有0,1,2,3; 0空白,1為邊,2為底邊,3方塊,4為投影 
int next_frame[nextHeight][nextWidth]={0}; // 後臺參數，框架中next_frame[y][x] x,y為x座標和y座標; 其值=>有0,1,2,3; 0空白,1為邊,2為底邊,3方塊
int Delay_time;
int score = 0;

//排行榜，有分數跟玩家名
class Rank
{
	public:
	string player;
	int score;
};

/* 陸紀豪 - 改顏色 */

#include<windows.h>
void SetColor(int color = 7)
{
  HANDLE hConsole;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole,color);
} 
int Color = 7;


//繪製函式
void draw(int x,int y,int uniframe,char *unit)
{
	if(uniframe == 1)
	{
		position(x,y);
		cout<<unit[0];
	}
	else if(uniframe == 2)
	{
		position(x,y);
		cout<<unit[1];
	}
	else if(uniframe == 3)
	{
		position(x,y);
		SetColor(Color);
		cout<<unit[2];
		SetColor();
	}
	else if(uniframe == 4)
	{
		position(x,y);
		cout<<unit[3];
	}
	else
	{
		position(x,y);
		cout<<' ';
	}

}
/**方塊架構 **/
int i_pos=5,j_pos=0; //方塊初始座標

//7種方塊
int Asset[7][4][4]={{{0,0,3,0},{0,0,3,0},{0,0,3,0},{0,0,3,0}},
  {{0,0,3,0},{0,3,3,0},{0,3,0,0},{0,0,0,0}},
  {{0,3,0,0},{0,3,3,0},{0,0,3,0},{0,0,0,0}},
  {{0,0,0,0},{0,0,3,0},{0,3,3,3},{0,0,0,0}},
  {{0,0,0,0},{0,3,3,0},{0,3,3,0},{0,0,0,0}},
  {{0,3,3,0},{0,0,3,0},{0,0,3,0},{0,0,0,0}},
{{0,3,3,0},{0,3,0,0},{0,3,0,0},{0,0,0,0}}};

//方塊落下 =>方塊放置位置(x,y)、清除方塊、方塊放置位置(x,y+1)

/* 杜以謙 - 投影 */ 
void AssetPos(int px,int py,int (&asset)[4][4])//px,py為起始位置
{
	for(int y = py; y < (py + 4); y++)
	{
		for(int x = px; x < (px + 4); x++)
		{
			if(frame[y][x] == 0 && asset[y - py][x - px] != 0)
			{
			
			    frame[y][x] = asset[y - py][x - px];
			    draw(x, y, frame[y][x], Unit);
			}
		}
	}
	
	int line_num = 0;
	for(int a = 21; a > 4; a--)
	{
		int down_a = a - 4;
		bool judge = false;
		for(int b = down_a; b < a; b++)
		{
			for(int c = px; c < px + 4; c++)
			{
				if((frame[b][c] == 3 or frame[b][c] == 2) && asset[b - down_a][c - px] == 3)
				{
					judge = true;
				}
			}
		}
		if(judge == true)
		{
			continue;
		}
		if(judge == false)
		{
			line_num = down_a;
			break;
		}
	}
	
	if(py < line_num)
	{
	    for(int y = line_num; y < line_num + 4; y++)
	    {
		    for(int x = px; x < (px + 4); x++)
		    {
			    if(frame[y][x] == 0 && asset[y - line_num][x - px] != 0)
			    {  
			        frame[y][x] = 4;
			        draw(x, y, frame[y][x], Unit);
			    }
		    }
	    }
    }
}

void cleanAsset(int px, int py, int (&asset)[4][4])
{
	int line_num = 0;
	for(int y = py; y < (py + 4); y++)
	{
		for(int x = px; x < (px + 4); x++)
		{
			if(asset[y - py][x - px] == 3)
			{
				frame[y][x] = 0;
				draw(x, y, frame[y][x], Unit);
			}
		}
	}
	
	for(int a = 21; a > 4; a--)
	{
		int down_a = a - 4;
		bool judge = false;
		for(int b = down_a; b < a; b++)
		{
			for(int c = px; c < px + 4; c++)
			{
				if((frame[b][c] == 3 or frame[b][c] == 2) && asset[b - down_a][c - px] == 3)
				{
					judge = true;
				}
			}
		}
		if(judge == false)
		{
			line_num = down_a;
			break;
		}
	}
	for(int y = line_num; y < line_num + 4; y++)
	{
		for(int x = px; x < (px + 4); x++)
		{
			if(frame[y][x] == 4)
			{
			    frame[y][x] = 0;
			    draw(x, y, frame[y][x], Unit);
			}
			
		}
	}
}

//判斷方塊是否碰撞
bool collision(int px,int py,int (&asset)[4][4])
{
for(int y=py; y<(py+4); y++)
	for(int x=px; x<(px+4); x++)
		if(frame[y][x]!=0 && asset[y-py][x-px]!=0)return 1;
return 0;
}

//////////////////////////////////////////////

//cout 左下下一個方塊
void PaintAfterAsset(int (&after_asset)[4][4])
{
	//AssetPos(2, nHeight + 3, after_asset);
	for (int i = 2; i < 6; i++)
	{
		for (int j = nHeight + 3; j < nHeight + 7; j++)
		{
			if (after_asset[j-nHeight-3][i-2] == 0)
			{
				position(i,j);
				cout << " ";
			}
			else
			{
				position(i,j);
				SetColor(Color);
				cout<<"+";
				SetColor();
			}
		}
	}
}

//////////////////////////////////////////////

/* Allen - 控制介面 */ 
//方塊順時針旋轉
void RotationClockwise(int px,int py,int (&asset)[4][4])
{
	int temp[4][4]={0};
	for(int x=0; x<4; x++)
		for(int y=0; y<4; y++)
			temp[x][y]=asset[3-y][x]; //轉90度
	cleanAsset(px,py,asset);
	if(!collision(px, py, temp))
		{
		for(int x=0; x<4; x++)
			for(int y=0; y<4; y++)
				asset[x][y]=temp[x][y];
		}
	AssetPos(px,py,asset);
}

//方塊逆時針旋轉
void RotationCounterClockwise(int px,int py,int (&asset)[4][4])
{
	int temp[4][4]={0};
	for(int x=0; x<4; x++)
		for(int y=0; y<4; y++)
			temp[x][y]=asset[y][3-x]; //逆轉90度
	cleanAsset(px,py,asset);
	if(!collision(px, py, temp))
		{
		for(int x=0; x<4; x++)
			for(int y=0; y<4; y++)
				asset[x][y]=temp[x][y];
		}
	AssetPos(px,py,asset);
}

//切換方塊
void ChangeAsset(int px, int py, int (&asset)[4][4], int(&after_asset)[4][4])
{
	int temp[4][4] = {0};
	for(int x=0; x<4; x++)
		for(int y=0; y<4; y++)
			temp[x][y]=after_asset[x][y]; 
	
	cleanAsset(px,py,asset);
	
	if(!collision(px, py, temp) && !collision(px+1, py, temp))
	{
		for(int x=0; x<4; x++)
			for(int y=0; y<4; y++)
				after_asset[x][y]=asset[x][y]; 

		for(int x=0; x<4; x++)
			for(int y=0; y<4; y++)
				asset[x][y]=temp[x][y];
	}
	PaintAfterAsset(after_asset);
	AssetPos(px,py,asset);
	
}

#define VK_X 88
#define VK_Z 90
//開始介面鍵盤控制
static DWORD count;
static INPUT_RECORD ir;

void start_key_Control(int &menu_choose,bool &chosen_bool)
{
	ReadConsoleInput(hIn,&ir,1,&count);
	switch(ir.Event.KeyEvent.wVirtualKeyCode)
	{
		case VK_UP: // 向上
		menu_choose--;
		break;
		case VK_DOWN: //向下
		menu_choose++;
		break;
		case VK_RETURN: // 按ENTER
		chosen_bool = true;
		break;
 
	} 	
}
//遊戲時鍵盤控制 往左、往右、旋轉
void key_Control(int (&asset)[4][4],int(&after_asset)[4][4])
{
	ReadConsoleInput(hIn,&ir,1,&count);
	switch(ir.Event.KeyEvent.wVirtualKeyCode)
	{
		case VK_LEFT://向左
		cleanAsset(i_pos,j_pos,asset);
		if(collision(i_pos-1,j_pos,asset))//判斷落到下一格是否發生碰撞
			AssetPos(i_pos,j_pos,asset);
		else
		{
			i_pos--;
			AssetPos(i_pos,j_pos,asset);
		}
		break;
		case VK_RIGHT://向右
		cleanAsset(i_pos,j_pos,asset);
		if(collision(i_pos+1,j_pos,asset))//判斷落到下一格是否發生碰撞
			AssetPos(i_pos,j_pos,asset);
		else
		{
			i_pos++;
			AssetPos(i_pos,j_pos,asset);
		}
		break;
		case VK_UP://旋轉
		RotationClockwise(i_pos,j_pos,asset);
		break;
        case VK_X:
        RotationClockwise(i_pos,j_pos,asset);
        break;
        case VK_Z:
        RotationCounterClockwise(i_pos,j_pos,asset);
        break;
		case VK_SHIFT:
		ChangeAsset(i_pos,j_pos,asset,after_asset);
		break;
		case VK_DOWN: //快速落下
		Delay_time=0;
		break;
	}
	Sleep(30);
}

//遊戲判定
bool Eliminate_Row_Asset(int y)//當一排都為方塊時，消除該牌方塊
{
	for(int x=1;x<nWidth-1;x++)
		if(frame[y][x]!=3)return 0;
	return 1;
}

//////////////////////////////////////////////

/* 陸紀豪 - Combo 加分 */

int Combo_times = 0;
int Combo_flag = false;

int Level = 0;

void Score(int count, int combo_counter, bool flag)
{
	
	if(flag){
		score+=count * (combo_counter + 1);
	}
	else{
		score+=count;
	}
	
	position(nWidth+16,nHeight-1);
	cout << score;
}

////////////////////////////

bool GameOver(int(&after_asset)[4][4])
{
	int high_i = 5, high_j = 0;
	if (collision(high_i,high_j,after_asset))
	{
		return 1;
	}
	else
	{
		for(int x=0; x<nWidth; x++)
		{
			if(frame[0][x]==3)
			return 1;
		}
		return 0;
	}
}

void SpeedMode()
{
	Delay_time = 250 - 0.04 * score;
}

/* Title Bar */

void titlebar(int now_score){
	if(now_score < 100){
		SetColor(15);
		Level = 1;
	}
	else if(now_score >= 100 and now_score < 300){
		SetColor(27);
		Level = 2;
	}
	else if(now_score >= 300 and now_score < 600){
		SetColor(42);
		Level = 3;		
	}
	else if(now_score >= 600 and now_score < 900){
		SetColor(49);
		Level = 4;		
	}
	else if(now_score >= 900 and now_score < 1200){
		SetColor(71);
		Level = 5;
	}
	else if(now_score >= 1200 and now_score < 1500){
		SetColor(87);
		Level = 6;
	}
	else if(now_score >= 1500 and now_score < 1800){
		SetColor(103);
		Level = 7;
	}
	else if(now_score >= 1800 and now_score < 2100){
		SetColor(113);
		Level = 8;
	}
	else if(now_score >= 2100 and now_score < 2400){
		SetColor(143);
		Level = 9;
	}
	else if(now_score >= 2400 and now_score < 2700){
		SetColor(158);
		Level = 10;
	}
	else if(now_score >= 2700 and now_score < 3000){
		SetColor(161);
		Level = 11;
	}
	else if(now_score >= 3000 and now_score < 3300){
		SetColor(181);
		Level = 12;
	}
	else if(now_score >= 3300 and now_score < 3600){
		SetColor(207);
		Level = 13;
	}
	else if(now_score >= 3600 and now_score < 3900){
		SetColor(223);
		Level = 14;
	}
	else if(now_score >= 3900 and now_score < 4200){
		SetColor(233);
		Level = 15;
	}
	else{
		SetColor(253);
		Level = 16;
	}
/*
	position(nWidth + 21 ,0);
	cout<< "__________________ ##__________";
	position(nWidth + 21 ,1);
	cout<< "_________________###*__________";
	position(nWidth + 21 ,2);
	cout<< "______________.*#####__________";
	position(nWidth + 21 ,3);
	cout<< "_____________*######___________";
	position(nWidth + 21 ,4);
	cout<< "___________*#######____________";
	position(nWidth + 21 ,5);
	cout<< "__________*########.___________";
	position(nWidth + 21 ,6);
	cout<< "_________*#########.___________";
	position(nWidth + 21 ,7);
	cout<< "_________*#######*##*__________";
	position(nWidth + 21 ,8);
	cout<< "________*#########*###_________";
	position(nWidth + 21 ,9);
	cout<< "_______*##########*__*##_______";
	position(nWidth + 21 ,10);
	cout<< "_____*###########*_____________";
	position(nWidth + 21 ,11);
	cout<< "____############_______________";
	position(nWidth + 21 ,12);
	cout<< "___*##*#########_______________";
	position(nWidth + 21 ,13);
	cout<< "___*_____########______________";
	position(nWidth + 21 ,14);
	cout<< "__________#######______________";
	position(nWidth + 21 ,15);
	cout<< "___________*######_____________";
	position(nWidth + 21 ,16);
	cout<< "____________*#####*____________";
	position(nWidth + 21 ,17);
	cout<< "______________*####*___________";
	position(nWidth + 21 ,18);
	cout<< "________________*####__________";
	position(nWidth + 21 ,19);
	cout<< "__________________*##*_________";
	position(nWidth + 21 ,20);
	cout<< "____________________*##________";
	position(nWidth + 21 ,21);
	cout<< "_____________________*##.______";
	position(nWidth + 21 ,22);
	cout<< "____________________.#####.____";
	position(nWidth + 21 ,23);
	cout<< "_________________.##########___";
	position(nWidth + 21 ,24);
	cout<< "________________.####*__*####__";
//	position(nWidth + 21 ,25);
//	cout<< "_______________________________";
*/

	position(nWidth + 21 ,0);
	cout<< "                  ..:::----------:::.                  ";
	position(nWidth + 21 ,1);
	cout<< "              ..:----------------------:.              ";
	position(nWidth + 21 ,2);
	cout<< "            .:----------------------------:.           ";
	position(nWidth + 21 ,3);
	cout<< "         .:---------=+++====-----------------.         ";
	position(nWidth + 21 ,4);
	cout<< "        :-------=++*###**+++++=----------------.       ";
	position(nWidth + 21 ,5);
	cout<< "      .-------=+=*#########***##++--------------:.     ";
	position(nWidth + 21 ,6);
	cout<< "     :-----==*+=+#**###*****#**###*+=-------------.    ";
	position(nWidth + 21 ,7);
	cout<< "    :-----+++*++**++++++====+**+*#****-------------.   ";
	position(nWidth + 21 ,8);
	cout<< "   :-----******+=======-::::---=+*+*##*-------------.  ";
	position(nWidth + 21 ,9);
	cout<< "  :-----***###*+===----:.....:::-=-+###+-------------. ";
	position(nWidth + 21 ,10);
	cout<< " .-----=######*++==--::::....:::::-=##%*-------------: ";
	position(nWidth + 21 ,11);
	cout<< " :-----+###%##**++==--::---=+**====+###+--------------.";
	position(nWidth + 21 ,12);
	cout<< " ------+%#%%###**+==+*+++++#**+==+=-*##+--------------:";
	position(nWidth + 21 ,13);
	cout<< ".------=%%%%%#**********##*#****+-:-=*+=---------------";
	position(nWidth + 21 ,14);
	cout<< ":-------%%%%%####*********:=+==--:-=--+=---------------";
	position(nWidth + 21 ,15);
	cout<< ":-------*%#%%%###***++++*=:.:=+=====--=----------------";
	position(nWidth + 21 ,16);
	cout<< ".--------##%%##*++++=++*+=-:::=**++++=-:---------------";
	position(nWidth + 21 ,17);
	cout<< " ---------*####**++++***+**+**+=+*+++=----------------:";
	position(nWidth + 21 ,18);
	cout<< " :--------=#*##****++***####*+++**++++=---------------.";
	position(nWidth + 21 ,19);
	cout<< " .---------+###************++=-+*+++++=--------------- ";
	position(nWidth + 21 ,20);
	cout<< "  :---------+**********##**++======+++---------------. ";
	position(nWidth + 21 ,21);
	cout<< "   -------------******+*******+==-=++---------------:  ";
	position(nWidth + 21 ,22);
	cout<< "   .-------------+*******+++==----+*=--------------:   ";
	position(nWidth + 21 ,23);
	cout<< "     :-------------+*****+++==+++++*+-------------:    ";
	position(nWidth + 21 ,24);
	cout<< "      :-------------****************+------------.     ";
	position(nWidth + 21 ,25);
	cout<< "       .------------+#*************++=:--------:       ";
	position(nWidth + 21 ,26);
	cout<< "         .----------=##***********++==+.:----:.        ";
	position(nWidth + 21 ,27);
	cout<< "           .:-------+###*********++++=+-  ..           ";
	position(nWidth + 21 ,28);
	cout<< "              .:----#%###*******+++++=+-               ";
	position(nWidth + 21 ,29);
	cout<< "              .:----#%###*******+++++=+-               ";
	position(nWidth + 21 ,30);
	cout<< "                 ..-+*####*******+==-:                 ";

//                      ..:::----------:::.                    
//                  ..:----------------------:.                
//                .:----------------------------:.             
//             .:---------=+++====-----------------.           
//            :-------=++*###**+++++=----------------.         
//          .-------=+=*#########***##++--------------:.       
//         :-----==*+=+#**###*****#**###*+=-------------.      
//        :-----+++*++**++++++====+**+*#****-------------.     
//       :-----******+=======-::::---=+*+*##*-------------.    
//      :-----***###*+===----:.....:::-=-+###+-------------.   
//     .-----=######*++==--::::....:::::-=##%*-------------:   
//     :-----+###%##**++==--::---=+**====+###+--------------.  
//     ------+%#%%###**+==+*+++++#**+==+=-*##+--------------:  
//    .------=%%%%%#**********##*#****+-:-=*+=---------------  
//    :-------%%%%%####*********:=+==--:-=--+=---------------  
//    :-------*%#%%%###***++++*=:.:=+=====--=----------------  
//    .--------##%%##*++++=++*+=-:::=**++++=-:---------------  
//     ---------*####**++++***+**+**+=+*+++=----------------:  
//     :--------=#*##****++***####*+++**++++=---------------.  
//     .---------+###************++=-+*+++++=---------------   
//      :---------+**********##**++======+++---------------.   
//       -------------******+*******+==-=++---------------:    
//       .-------------+*******+++==----+*=--------------:     
//         :-------------+*****+++==+++++*+-------------:      
//          :-------------****************+------------.       
//           .------------+#*************++=:--------:         
//             .----------=##***********++==+.:----:.          
//               .:-------+###*********++++=+-  ..             
//                  .:----#%###*******+++++=+-                 
//                     ..-+*####*******+==-:                   
//                           .==***+=-                     


	SetColor();
}



/* 主要函式開始 */

int main()
{
	Control_HANDLE_Unit();
	// 遊戲背景
	
	//開始介面
	bool chosen_bool = false;
	int menu_choose = 500;
	int mode_Cnt = 5;
	int mode = 0; // mode:1,2,3,4,5
	cout << " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 俄 羅 斯 方 塊 =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
	cout << "* 操作方式:                                                                   *" << endl;
	cout << "*                      <-- , --> :   左右移動                                 *" << endl;
	cout << "*                          z     : 逆時針旋轉方塊                             *" << endl;
	cout << "*                        x , 上  : 順時針旋轉方塊                             *" << endl;
	cout << "*                        Shift   :   切換方塊                                 *" << endl;
	cout << "* 選擇地圖:                                                                   *" << endl;
	cout << "*                                                                             *" << endl;
	for(int i = 0; i < 5; i++)
			{
				if(mode != i)
				{
					cout << "*                               ";
					cout << "   " << " 地圖 " << i << "                                    *" << endl; 
				}
				else
				{
					cout << "*                               ";
					cout << "-->" << " 地圖 " << i << "                                    *" << endl;
				}
				cout <<"*                                                                             *"<< endl;
			}
			for(int i = 0; i < 79; i++)
			{
				if(i%2 == 0)
					cout << "*";
				else
					cout << "~";
			}
	while(1)
	{
		if(kbhit())
		{
			ReadConsoleInput(hIn,&ir,1,&count);
			start_key_Control(menu_choose, chosen_bool);
			mode = menu_choose % mode_Cnt;
			system("cls");
			cout << " =============================== 俄 羅 斯 方 塊 ===============================" << endl;
	cout << "* 操作方式:                                                                   *" << endl;
	cout << "*                      <-- , --> :   左右移動                                 *" << endl;
	cout << "*                          z     : 逆時針旋轉方塊                             *" << endl;
	cout << "*                        x , 上  : 順時針旋轉方塊                             *" << endl;
	cout << "*                        Shift   :   切換方塊                                 *" << endl;
	cout << "* 選擇地圖:                                                                   *" << endl;
	cout << "*                                                                             *" << endl;
			for(int i = 0; i < 5; i++)
			{
				if(mode != i)
				{
					cout << "*                               ";
					cout << "   " << " 地圖 " << i << "                                    *" << endl; 
				}
				else
				{
					cout << "*                               ";
					cout << "-->" << " 地圖 " << i << "                                    *" << endl;
				}
				cout <<"*                                                                             *"<< endl;
			}
			for(int i = 0; i < 79; i++)
			{
				if(i%2 == 0)
					cout << "*";
				else
					cout << "~";
			}
		}
		if(chosen_bool == true)
		{
			break;
		}
		else
		{
			Sleep(Delay_time);
		}
	}
	system("cls");
	titlebar(score);

	
	// 主視窗 
	for(int x=0; x<nWidth; x++) // 底邊框
		frame[nHeight-1][x]=2;
	for(int y=0; y<nHeight; y++) // 邊框
	{
		frame[y][0]=1;
		frame[y][nWidth-1]=1;
	}
	for(int y=0; y<nHeight; y++)
		for(int x=0; x<nWidth; x++)
			draw(x,y,frame[y][x],Unit);
	for(int y=0; y<nHeight; y++)
	{
		position(nWidth + 1,y);
		cout<<'>'<<y;
	}
	
	if(mode == 1)
	{
		for(int a = 16; a < 20; a++)
		{
			for(int b = 0; b < 16; b++)
			{
				if((a + b) % 2 == 0 and frame[a][b] == 0)
				{
					frame[a][b] = 3;
				}
			}
		}
		for(int y=0; y<nHeight; y++)
		{
  			for(int x=0; x<nWidth; x++)
  			{
   				if (frame[y][x] == 3)
   				{
    				Color = 191;
    				draw(x, y, frame[y][x], Unit);
   				}
   
   				else
   				{
    				SetColor(7);
    				draw(x,y,frame[y][x],Unit);
   				}
  			}
 		}
	}
	
	else if (mode == 2)
	{
		int start1 = 7;
		int start2 = 5;
		int ct1 = 2;
		int ct2 = 6;
		
		for (int a = 14; a < 20; a++)
		{
			if (a < 17)
			{
				for (int b = start1; b < start1+ct1; b++)
				{
					frame[a][b] = 3;
				}
				start1--;
				ct1 += 2;
			}
			else
			{
				for (int b = start2; b < start2+ct2; b++)
				{
					frame[a][b] = 3;
				}
				start2++;
				ct2 -= 2;
			}
		}
		
		for(int y=0; y<nHeight; y++)
		{
  			for(int x=0; x<nWidth; x++)
  			{
   				if (frame[y][x] == 3)
   				{
    				Color = 191;
    				draw(x, y, frame[y][x], Unit);
   				}
   
   				else
   				{
    				SetColor(7);
    				draw(x,y,frame[y][x],Unit);
   				}
  			}
 		}
		
	}

	
	
	// 看下一個方塊的區域
	for(int y=nHeight; y < nHeight + nextHeight; y++)
	{
		position(0, y);
		cout << '|';
		position(nextWidth, y);
		cout << '|';
	}
	position(0, nHeight + nextHeight + 1);
	cout << "Next One";
	
	for(int x=0; x <= nextWidth; x++)
	{	position(x, nHeight);
		cout << '=';
		position(x, nHeight + nextHeight);
		cout << '=';
	}
	
	
	// 計分板
	position(nWidth+10,nHeight-3);
	cout<<"Level:";
	position(nWidth+10,nHeight-2);
	cout<<"Combo:";
	position(nWidth+10,nHeight-1);
	cout<<"Score:";
	Score(0, Combo_times, Combo_flag);


	//排行榜
	ifstream ranking("ranking.txt");

	Rank ranks[5];  // 記錄前五名
	for (int i = 0; i < 5; i ++)
	{
		ranks[i].player = "theplayer";
		ranks[i].score = 0;
	}

	int counter = 0;
	for (int i = 0; i < 5; i ++)
	{
		string aline;
		getline(ranking, aline);
		ranks[counter].player = aline;
		getline(ranking, aline);
		ranks[counter].score = atoi(aline.c_str());
		counter += 1;
		
	}
	position(nWidth+80, nHeight-20);
	cout << "RANK";

	for (int i = 0; i < 5; i ++)
	{
		position(nWidth+80, nHeight-18+2*i);
		cout << "Rank" << i+1 << "  " << ranks[i].player;
		position(nWidth+98, nHeight-18+2*i);
		cout << ranks[i].score;
	}
	
	ranking.close();

	
	// 遊戲開始 
	// 放入方塊
	int asset[4][4]={0}; // 目前的方塊
	int after_asset[4][4]={0}; // 下一個方塊
	bool begin = true;
	while(chosen_bool)
	{
		i_pos=5,j_pos=0;//起始座標i,j
		SpeedMode();//速度模式
		srand(time(NULL));
		int random = rand() % 7;
		for(int i=0; i<4; i++)
		{	
			if(begin){
				for(int j=0; j<4; j++)
					asset[i][j]=Asset[random][i][j]; // 這一回合的方塊會是上一回合的下一個方塊
				
			}
			else{
				for(int j=0; j<4; j++)
					asset[i][j]=after_asset[i][j]; // 這一回合的方塊會是上一回合的下一個方塊 	
			}
				 
			if(random == 0){
				Color = 223;
			}
			else if(random == 1){
				Color = 224;
			}
			else if(random == 2){
				Color = 241;
			}
			else if(random == 3){
				Color = 176;
			}
			else if(random == 4){
				Color = 135;
			}
			else if(random == 5){
				Color = 155;
			}
			else if(random == 6){
				Color = 107;
			}
		}
		
		begin = false; // 判斷是否為第一次 
		int another_random = rand() % 7;
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				after_asset[i][j]=Asset[another_random][i][j]; // 這回合的下一個方塊，會從七種方塊裡面隨機選一種
			}
		}
		
		PaintAfterAsset(after_asset);
		AssetPos(i_pos,j_pos,asset);

		//方塊落下
		while(1)
		{
			while(!kbhit())
			{
				Sleep(Delay_time);
				cleanAsset(i_pos,j_pos,asset);
				if(collision(i_pos,j_pos+1,asset))//有碰撞
				{	
					AssetPos(i_pos,j_pos,asset);
					break;
				}
				j_pos++;
				AssetPos(i_pos,j_pos,asset);
				Sleep(Delay_time);
				if(kbhit())
				{
					ReadConsoleInput(hIn,&ir,1,&count);
					key_Control(asset,after_asset);
					PaintAfterAsset(after_asset);
				}
			}
			if(kbhit())
			{
				ReadConsoleInput(hIn,&ir,1,&count);
				key_Control(asset,after_asset);
				PaintAfterAsset(after_asset);
			}
			cleanAsset(i_pos,j_pos,asset);
			if(collision(i_pos,j_pos+1,asset))
			{	
				AssetPos(i_pos,j_pos,asset);
				break;
			}
			PaintAfterAsset(after_asset);
			AssetPos(i_pos,j_pos,asset);
		}
		
		// 方塊碰到底之後看有沒有條數可以消掉
		
		bool now_combo_flag = false; // 當前這個方塊視為一回合的話，查看此回合有沒有消過條數
		for(int y=nHeight-2; y>0; y--) // 削掉方塊並計分
		{
			if(Eliminate_Row_Asset(y))
			{	
				for(int x=1; x<nWidth-1; x++)
				{
					frame[y][x]=0;
					draw(x,y,frame[y][x],Unit);
				}
				Sleep(100);
				Score(100, Combo_times, Combo_flag);
				
				for(int j=y; j>0; j--)//上面方塊掉下來
				{
					for(int x=1; x<nWidth-1; x++)
					{
						frame[j][x]=frame[j-1][x];
						draw(x,j,frame[j][x],Unit);
					}
				}
				y++;
				
				Combo_times += 1;
				Combo_flag = true;
				now_combo_flag = true;
			}
			
			if(y == 1){
				if(now_combo_flag){

				}
				else{
					Combo_flag = false;
					Combo_times = 0;
				}
			}
				position(nWidth+16,nHeight-3);
				cout << Level;
				position(nWidth+16,nHeight-2);
				cout << Combo_times;
			
		}
		if(GameOver(after_asset)) // 結束遊戲條件判斷 
		{
			position(nWidth + 55 ,nHeight-6);
			cout<<"GAMEOVER!!";
			
			// 判斷是否更新排行榜
			string playerName;

			if (score > ranks[4].score)
			{
				position(nWidth+55, nHeight-5);
				cout << "Please enter your name: ";
				getline(cin, playerName);
			}

			if (score > ranks[0].score)
			{
				for (int i = 4; i >0 ; i --)
				{
					ranks[i].score = ranks[i-1].score;
					ranks[i].player = ranks[i-1].player;
				}
				
				ranks[0].score = score;
				ranks[0].player = playerName;
				
			}

			else if (score > ranks[1].score)
			{
				for (int i = 4; i > 1; i --)
				{
					ranks[i].score = ranks[i-1].score;
					ranks[i].player = ranks[i-1].player;
				}
				ranks[1].score = score;
				ranks[1].player = playerName;
			}

			else if (score > ranks[2].score)
			{
				for (int i = 4; i > 2; i --)
				{
					ranks[i].score = ranks[i-1].score;
					ranks[i].player = ranks[i-1].player;
				}
				ranks[2].score = score;
				ranks[2].player = playerName;
			}

			else if (score > ranks[3].score)
			{
				ranks[4].score = ranks[3].score;
				ranks[4].player = ranks[3].player;
				ranks[3].score = score;
				ranks[3].player = playerName;
			}

			else if (score > ranks[4].score)
			{
				ranks[4].score = score;
				ranks[4].player = playerName;
			}
			
			ofstream ranking("ranking.txt");
			for (int i = 0; i < 5; i ++)
			{
				ranking << ranks[i].player << "\n";
				ranking << ranks[i].score << "\n";
			}
			ranking.close();
			break;
		}
		
		// 更新資訊：圖片, 下一個方塊 
		// cleanAsset(2, nHeight + 3, after_asset);
		titlebar(score);
	}
	system("PAUSE");
	return 0;
}








