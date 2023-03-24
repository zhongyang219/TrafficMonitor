#include "stdafx.h"
#include "CalendarHelper.h"


CCalendarHelper::CCalendarHelper()
{
}


CCalendarHelper::~CCalendarHelper()
{
}

bool CCalendarHelper::IsLeapYear(int year)
{
	return ((0 == year % 4 && 0 != year % 100) || 0 == year % 400);
}

int CCalendarHelper::CaculateWeekDay(int y, int m, int d)
{
	if (m <= 2)
	{
		m += 12;
		y--;
	}
	return (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
}

int CCalendarHelper::DaysInMonth(int year, int month)
{
	bool leap{ IsLeapYear(year) };
	switch (month)
	{
	case 2:
		if (leap) return 29;
		else return 28;
	case 4: case 6: case 9: case 11:
		return 30;
	default:
		return 31;
	}
}

void CCalendarHelper::GetCalendar(int year, int month, DayTraffic calendar[CALENDAR_HEIGHT][CALENDAR_WIDTH], FirstDayOfWeek firstDayOfWeek)
{
	memset(calendar, 0, sizeof(DayTraffic)*CALENDAR_HEIGHT*CALENDAR_WIDTH);
	int days{ DaysInMonth(year, month) };
	int first_week_day{ CaculateWeekDay(year, month, 1) };
	switch (firstDayOfWeek)
	{
		case FirstDayOfWeek::SATURDAY:
		{
			first_week_day++;
			if (first_week_day > 6)
				first_week_day = 0;
			break;
		}
		case FirstDayOfWeek::MONDAY:
		{
			first_week_day --;
			if (first_week_day < 0)
				first_week_day = 6;
			break;
		}
		default:
			break;
	}
	int i{}, j{};
	for (int n{}; n < 37; n++)
	{
		if (n < first_week_day)
		{
			calendar[i][j].day = 0;
		}
		else
		{
			int day = n - first_week_day + 1;
			if (day <= days)
				calendar[i][j].day = day;
		}
		j++;
		if (j >= 7)
		{
			j = 0;
			i++;
		}
	}
}
