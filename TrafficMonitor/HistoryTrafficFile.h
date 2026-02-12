#pragma once
#include "CommonData.h"
class CHistoryTrafficFile
{
public:
	CHistoryTrafficFile(const wstring& file_path);
	~CHistoryTrafficFile();

	void Save() const;
	void SaveTodayOnly() const;	// 仅更新第一行（lines计数）和今天的记录（第二行），用于频繁保存时减少I/O
	void Load();
	void LoadSize();			//仅读取文件的大小
	void Merge(const CHistoryTrafficFile& history_traffic, bool ignore_same_data = false);		//合并另一个CHistoryTrafficFile对象。如果ignore_same_data为true，则忽略相同日期的项，否则将相同日期的流量数据相加
	void OnDateChanged();		//日期改变时调用，将今天的记录移到历史记录，创建新的今天的记录

	const wstring& GetFilePath() const { return m_file_path; }
	const void SetFilePath(const wstring& file_path) { m_file_path = file_path; }
	
	// 获取完整流量列表（用于统计功能），返回deque引用以兼容现有代码
	deque<HistoryTraffic>& GetTraffics() { if (m_cache_dirty) UpdateCache(); return m_traffics_cache; }
	const deque<HistoryTraffic>& GetTraffics() const { if (m_cache_dirty) UpdateCache(); return m_traffics_cache; }
	
	// 获取今天的记录（第一条）
	HistoryTraffic& GetTodayTraffic() { return m_today_traffic; }
	const HistoryTraffic& GetTodayTraffic() const { return m_today_traffic; }
	
	// 获取历史记录链表
	const list<HistoryTraffic>& GetHistoryTraffics() const { return m_history_traffics; }
	
	__int64 GetTodayUpTraffic() const { return m_today_up_traffic; }
	__int64 GetTodayDownTraffic() const { return m_today_down_traffic; }
	size_t Size() const { return m_size; }

private:
	void MormalizeData();		//将历史流量数据排序并合并相同项
	bool IsTodayRecord() const;	//检查今天的记录日期是否正确
	void UpdateCache() const;	//更新缓存（用于统计功能）
	void WriteTrafficRecord(ofstream& file, const HistoryTraffic& traffic) const;	//写入一条流量记录到文件
	HistoryTraffic CreateTodayTraffic() const;	//创建今天的记录（日期为当前日期，流量为0）
	void InvalidateCache() const { m_cache_dirty = true; }	//标记缓存过期

private:
	wstring m_file_path;
	HistoryTraffic m_today_traffic;        // 今天的记录（单独存储，频繁更新）
	list<HistoryTraffic> m_history_traffics;	// 历史记录链表（按日期从大到小排序）
	mutable deque<HistoryTraffic> m_traffics_cache;	// 缓存：合并后的完整列表（用于统计功能，按需更新）
	mutable bool m_cache_dirty{ true };	// 缓存是否过期（需要更新）
	__int64 m_today_up_traffic{};	//今天已使用的上传流量
	__int64 m_today_down_traffic{};	//今天已使用的下载流量
	size_t m_size{};				//流量数据的数量
};

