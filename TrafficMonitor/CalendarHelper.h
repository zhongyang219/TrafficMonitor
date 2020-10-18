#pragma once
#define CALENDAR_WIDTH 7
#define CALENDAR_HEIGHT 6

struct DayTraffic
{
	int day;
	__int64 up_traffic;
	__int64 down_traffic;
	bool mixed;
	CRect rect;

	__int64 traffic() const
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
	//如果sunday_first为true，则将周日作为每周的第一天，否则，将周一作为每周的第一天
	static void GetCalendar(int year, int month, DayTraffic calendar[CALENDAR_HEIGHT][CALENDAR_WIDTH], bool sunday_first = true);

};

