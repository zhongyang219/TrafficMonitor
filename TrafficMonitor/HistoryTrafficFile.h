#pragma once
#include "CommonData.h"
class CHistoryTrafficFile
{
public:
	CHistoryTrafficFile(const wstring& file_path);
	~CHistoryTrafficFile();

	void Save() const;
	void Load();
	void LoadSize();			//仅读取文件的大小
	void Merge(const CHistoryTrafficFile& history_traffic, bool ignore_same_data = false);		//合并另一个CHistoryTrafficFile对象。如果ignore_same_data为true，则忽略相同日期的项，否则将相同日期的流量数据相加

	const wstring& GetFilePath() const { return m_file_path; }
	const void SetFilePath(const wstring& file_path) { m_file_path = file_path; }
	deque<HistoryTraffic>& GetTraffics() { return m_history_traffics; }
	__int64 GetTodayUpTraffic() const { return m_today_up_traffic; }
	__int64 GetTodayDownTraffic() const { return m_today_down_traffic; }
	size_t Size() { return m_size; }

private:
	void MormalizeData();		//将历史流量数据排序并合并相同项

private:
	wstring m_file_path;
	deque<HistoryTraffic> m_history_traffics;	//储存历史流量
	__int64 m_today_up_traffic{};	//今天已使用的上传流量
	__int64 m_today_down_traffic{};	//今天已使用的下载流量
	size_t m_size{};				//流量数据的数量
};

