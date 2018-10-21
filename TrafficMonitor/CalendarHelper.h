#pragma once
#define CALENDAR_WIDTH 7
#define CALENDAR_HEIGHT 6

struct DayTraffic
{
	int day;
	int up_traffic;
	int down_traffic;
	bool mixed;
	CRect rect;

	int traffic() const
	{
		return up_traffic + down_traffic;
	}
};

class CCalendarHelper
{
public:
	CCalendarHelper();
	~CCalendarHelper();

	//是否为闰年
	static bool IsLeapYear(int year);
	//根据日期计算星期(0~6：星期日~星期六)
	static int CaculateWeekDay(int y, int m, int d);
	//计算一个月有多少天
	static int DaysInMonth(int year, int month);

	//获取指定月份的日历数据，并保存在数组calendar中
	static void GetCalendar(int year, int month, DayTraffic calendar[CALENDAR_HEIGHT][CALENDAR_WIDTH]);

};

