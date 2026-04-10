#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cstring>
#include<vector>
using namespace std;
constexpr auto swidth = 850;
constexpr auto sheight = 950;
constexpr unsigned int HP = 3;
constexpr auto HURTTIME = 1000;
#define ENEMY_NUM 6
class Button :public RECT
{	
public:
	int COLOR=BLUE;
	LPCTSTR text;
public:
	Button(const int& l=0, const int& t=0, const int& r=0, const int& b=0)
	{ 
		left = l; top = t; right = r; bottom = b;
	}
	void SetColor(const int& color) { COLOR = color; }
	void SetText(LPCTSTR& other) { text = other; }
	void Move(const int&l,const int&t,const int&r,const int&b){ left = l; top = t; right = r; bottom = b; }
	void Display()
	{
		setfillcolor(COLOR);
		fillrectangle(left - 10, top - 10, right + 10, bottom + 10);
		settextcolor(YELLOW);
		outtextxy(left, top, text);
	}
	bool PointInRect(const int& x, const int& y)
	{
		return (x >=left-10 && x <= right+10 && y >= top-10 && y <=bottom+10);
	}
};
void Welcome()
{	
	BeginBatchDraw();
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE img;
	loadimage(&img, _T(".\\images\\welcome.jpeg"));
	img.Resize(swidth,sheight);
	putimage(0, 0, &img);
	settextstyle(60, 0, _T("hxj"));
	settextcolor(BLACK);
	LPCTSTR title = _T("飞机大战");
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 10+100, title);
	settextstyle(40, 0, _T("hxj"));
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("结束游戏");
	Button tplayr;
	tplayr.SetText(tplay);
	tplayr.Move(swidth / 2 - textwidth(tplay) / 2, sheight / 5+200, swidth / 2 + textwidth(tplay) / 2, sheight / 5 +200+ textheight(tplay));
	tplayr.Display();
	Button texitr;
	texitr.SetText(texit);
	texitr.Move(swidth / 2 - textwidth(texit) / 2, sheight / 3 + 200, swidth / 2 + textwidth(texit) / 2, sheight / 3 + 200 + textheight(texit));
	texitr.Display();
	FlushBatchDraw();
	bool play_mess = false;
	bool exit_mess = false;
	while (true)
	{
		FlushBatchDraw();
		ExMessage mess;
		mess = getmessage(EX_MOUSE);
		bool now_play_mess = tplayr.PointInRect(mess.x, mess.y);
		bool now_exit_mess = texitr.PointInRect(mess.x, mess.y);
		if (now_play_mess != play_mess)
		{
			play_mess = now_play_mess;
			tplayr.SetColor(now_play_mess?RED:BLUE);
			tplayr.Display();
			FlushBatchDraw();
		}
		if (now_exit_mess != exit_mess)
		{
			exit_mess = now_exit_mess;
			texitr.SetColor(now_exit_mess ? RED : BLUE);
			texitr.Display();
			FlushBatchDraw();
		}
		if (mess.message == WM_LBUTTONDOWN)
		{
			if (now_play_mess)
			{
				EndBatchDraw();
				return;
			}
			if (now_exit_mess)
			{
				EndBatchDraw();
				exit(0);
			}
		}
	}
}

class BK
{
public:
	BK(IMAGE& img) :img(img),y(-sheight) {}
	void Show()
	{
		if (y == 0) { y = -sheight; }
		y += 2;
		putimage(0, y, &img);
	}

private:
	IMAGE &img;
	int  y;
};

bool RectDuangRect(RECT& rect1, RECT& rect2)
{
	if (rect1.right<=rect2.left || rect1.left>=rect2.right|| rect1.bottom <= rect2.top || rect1.top >= rect2.bottom)return false;
	return true;
}

bool CrossX(RECT& rect1, RECT& rect2)
{
	if (rect1.right<rect2.left || rect1.left>rect2.right)return false;
	return true;
}
class Hero
{
public:
	Hero(IMAGE &img):img(img),hp(HP)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = sheight;
	}
	void Show()
	{
		setfillcolor(RED);
		putimage(rect.left, rect.top, &img);
		fillrectangle(rect.left, rect.top+15,rect.left+( rect.right-rect.left) / HP * hp, rect.top);
	}
	void Control()
	{
		ExMessage mess;
		if (peekmessage(&mess, EM_MOUSE))
		{
			rect.left = mess.x - img.getwidth() / 2;
			rect.top = mess.y - img.getheight() / 2;
			rect.right = rect.left + img.getwidth();
			rect.bottom = rect.top + img.getheight();
		}
	}
	bool hurt()
	{
		--hp;
		return hp == 0 ? false : true;
	}
	RECT& GetRect() { return rect; }
private:
	IMAGE img;
	RECT rect;
	unsigned int hp;
};

class Enemy
{
public:
	Enemy(IMAGE* img,int x,IMAGE*boom) :img(img),bullet(0),isdie(false),boomsum(0)
	{
		booms[0] = boom[0];
		booms[1] = boom[1];
		booms[2] = boom[2];
		rect.left = x;
		rect.right = rect.left + img->getwidth();
		rect.top = -img->getheight();
		rect.bottom = 0;
		speed = abs(rand() % 4) + 2;
	}
bool Show()
{
	if (isdie)
	{
		if (boomsum == 3)return false;
		putimage(rect.left, rect.top, booms + boomsum);
		boomsum++;
		return true;
	}
	if (rect.top >= sheight)return false;
	rect.top += speed; rect.bottom += speed;
	putimage(rect.left, rect.top, img);
	return true;
}
int& Getbullet()
{
	return bullet;
}
RECT& GetRect() { return rect; }
int GetSpeed() { return speed; }
void Isdie(bool x)
{
	isdie = x;
}
private:
	IMAGE* img;
	RECT rect;
	IMAGE booms[3];
	int speed;
	int bullet;
	bool isdie;
	int boomsum;
};

class Bullet
{
public:
	Bullet(IMAGE* img, RECT pr) :img(img)
	{
		rect.left = (pr.left + pr.right - img->getwidth()) / 2;
		rect.right = rect.left + img->getwidth();
		rect.bottom = pr.top;
		rect.top = rect.bottom - img->getheight();
	}

	virtual bool Show()
	{
		if (rect.bottom <= 0)return false;
		rect.top -= 3;
		rect.bottom -= 3;
		putimage(rect.left, rect.top, img);
		return true;
	}
	RECT& GetRect() { return rect; }
protected:
	IMAGE* img;
	RECT rect;
};

class EBullet :public Bullet
{
public:
	EBullet(IMAGE*img,Enemy*e):Bullet(img,e->GetRect()),speed(e->GetSpeed()+1)
	{	
		rect.top= e->GetRect().bottom;
		rect.bottom = rect.top + img->getheight();
	}
	virtual bool Show()
	{
		if (rect.top >= sheight)return false;
		rect.bottom += speed; rect.top += speed;
		putimage(rect.left, rect.top, img);
		return true;
	}
private:
	int speed;
};
bool AddEnemy(vector<Enemy*>& es, IMAGE& img,IMAGE*booms)
{
	Enemy* tmp=new Enemy(&img, abs(rand() % (swidth - img.getwidth())),booms);
	for (auto& i : es)
	{
		if (CrossX(i->GetRect(), tmp->GetRect()))return false;
	}
	es.push_back(tmp);
	return true;
}

void Over(unsigned long long kill)
{
	setbkcolor(WHITE); setbkmode(OPAQUE);
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("击杀数:%llu"), kill);
	settextcolor(RED);
	outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);
	LPCTSTR info = _T("按enter返回");
	settextstyle(20, 0, _T("hxj"));
	outtextxy(swidth - textwidth(info), sheight - textheight(info), info);

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_KEY);
		if (mess.vkcode == VK_RETURN)
		{
			return;
		}
	}

}
bool Play()
{
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;
	IMAGE heroimg, enemyimg, bkimg, bulletimg, eboom[3];
	loadimage(&heroimg, _T(".\\images\\me1.png"));
	loadimage(&enemyimg, _T(".\\images\\enemy1.png"));
	loadimage(&bkimg, _T(".\\images\\bk2.png"), swidth);
	loadimage(&bulletimg, _T(".\\images\\bullet1.png"));
	loadimage(&eboom[0], _T(".\\images\\enemy1_down1.png"));
	loadimage(&eboom[1], _T(".\\images\\enemy1_down2.png"));
	loadimage(&eboom[2], _T(".\\images\\enemy1_down3.png"));
	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);
	vector<Enemy*>es;
	vector<Bullet*>bs;
	vector<EBullet*>ebs;
	int bsing = 0;
	int ebsing = 0;
	clock_t hurtlast = clock();
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		AddEnemy(es, enemyimg,eboom);
	}
	unsigned long long kill = 0;
	while (is_play)
	{
		bool space_state_now = GetAsyncKeyState(VK_SPACE) & 0x8000;
		if (space_state_now)
		{
			while (GetAsyncKeyState(VK_SPACE) & 0x8000) { Sleep(10); }
			TCHAR t[]=_T("游戏暂停");
			settextcolor(RED); outtextxy(swidth / 2-textwidth(t)/2, sheight / 5, t);
			while (true)
			{
				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				{
					while (GetAsyncKeyState(VK_SPACE) & 0x8000) { Sleep(10); }
					break;
				}
				Sleep(10);
			}
		}
			++bsing;
			if (bsing == 20)
			{
				bsing = 0;
				bs.push_back(new Bullet(&bulletimg, hp.GetRect()));
			}
			BeginBatchDraw();
			bk.Show();
			Sleep(4);
			flushmessage();
			Sleep(1);
			hp.Control();
			hp.Show();
			for (auto i = bs.begin(); i != bs.end();)
			{
				if (!(*i)->Show())
				{
					delete(*i);
					i = bs.erase(i);
				}
				else { ++i; }
			}
			for (auto i = ebs.begin(); i != ebs.end();)
			{
				if (!(*i)->Show())
				{
					delete(*i);
					i = ebs.erase(i);
				}
				else { ++i; }
			}
			auto it = es.begin();
			while (it != es.end())
			{	
				if (++(*it)->Getbullet() == 80)
				{ 
					(*it)->Getbullet() = 0;
					ebs.push_back(new EBullet(&bulletimg, (*it)));
				};
				
				if (RectDuangRect((*it)->GetRect(), hp.GetRect()))
				{
					if (clock() - hurtlast >= HURTTIME)
					{
						hurtlast = clock();
						is_play = hp.hurt();
					}

				}
				auto bit = bs.begin();
				while (bit != bs.end())
				{
					if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect()))
					{
						(*it)->Isdie(true);
						delete(*bit);
						bs.erase(bit);
						++kill;
						break;
					}
					else
					{
						++bit;
					}
				}
				auto ebit = ebs.begin();
				while (ebit != ebs.end())
				{
					if (RectDuangRect((*ebit)->GetRect(), hp.GetRect()))
					{
						if (clock() - hurtlast >= HURTTIME)
						{
							hurtlast = clock();
							is_play = hp.hurt();
						}
						break;
					}
					else
					{
						++ebit;
					}
				}
				if (it != es.end())
				{
					if (!(*it)->Show())
					{
						delete (*it);
						it = es.erase(it);
					}
					else
						++it;
				}
			}
			for (int i = 0; i < ENEMY_NUM - (int)es.size(); ++i)
			{
				AddEnemy(es, enemyimg,eboom);
			}
		EndBatchDraw();
	}
	Over(kill);
	return true;
}
int main()
{
	initgraph(swidth,sheight);

	bool is_live = true;
	while (is_live)
	{
		Welcome();
		is_live = Play();
	}
	closegraph();
}