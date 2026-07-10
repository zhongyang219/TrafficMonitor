#include "stdafx.h"
#include "HistoryTrafficFile.h"
#include "Common.h"

CHistoryTrafficFile::CHistoryTrafficFile(const wstring& file_path)
	: m_file_path(file_path)
{
}

CHistoryTrafficFile::~CHistoryTrafficFile()
{
}

HistoryTraffic CHistoryTrafficFile::CreateTodayTraffic() const
{
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	HistoryTraffic today_traffic;
	today_traffic.year = current_time.wYear;
	today_traffic.month = current_time.wMonth;
	today_traffic.day = current_time.wDay;
	today_traffic.up_kBytes = 0;
	today_traffic.down_kBytes = 0;
	today_traffic.mixed = false;
	return today_traffic;
}

void CHistoryTrafficFile::WriteTrafficRecord(ofstream& file, const HistoryTraffic& traffic) const
{
	char buff[64];
	if (traffic.mixed)
	{
		sprintf_s(buff, "%.4d/%.2d/%.2d %llu", traffic.year, traffic.month, 
			traffic.day, traffic.down_kBytes);
	}
	else
	{
		sprintf_s(buff, "%.4d/%.2d/%.2d %llu/%llu", traffic.year, traffic.month, 
			traffic.day, traffic.up_kBytes, traffic.down_kBytes);
	}
	file << buff << "\n";
}

void CHistoryTrafficFile::UpdateCache() const
{
	// 更新缓存：合并今天的记录和历史记录链表
	m_traffics_cache.clear();
	m_traffics_cache.push_front(m_today_traffic);
	m_traffics_cache.insert(m_traffics_cache.end(), m_history_traffics.begin(), m_history_traffics.end());
	m_cache_dirty = false; // 标记缓存已更新
}

void CHistoryTrafficFile::Save() const
{
	ofstream file{ m_file_path };
	if (!file.is_open())
	{
		return;
	}

	char buff[64];
	
	// 第一行：总记录数（今天的记录 + 历史记录）
	size_t total_size = 1 + m_history_traffics.size();
	sprintf_s(buff, "lines: \"%u\"", static_cast<unsigned int>(total_size));
	file << buff << "\n";

	// 第二行：今天的记录
	WriteTrafficRecord(file, m_today_traffic);

	// 第三行及之后：历史记录链表
	for (const auto& history_traffic : m_history_traffics)
	{
		WriteTrafficRecord(file, history_traffic);
	}

	file.close();
}

bool CHistoryTrafficFile::IsTodayRecord() const
{
	// 检查今天的记录日期是否正确
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	
	return (m_today_traffic.year == current_time.wYear &&
			m_today_traffic.month == current_time.wMonth &&
			m_today_traffic.day == current_time.wDay);
}

void CHistoryTrafficFile::SaveTodayOnly() const
{
	// 增量保存：只更新第一行（lines计数）和第二行（今天的记录）
	// 前提：今天的记录日期正确（程序自己维护，无需读取文件判断）
	
	// 如果今天的记录日期不正确，说明日期刚改变，使用完整保存
	if (!IsTodayRecord())
	{
		Save();
		return;
	}

	// 文件不存在时，使用完整保存（首次保存）
	if (!CCommon::FileExist(m_file_path.c_str()))
	{
		Save();
		return;
	}

	// 读取文件剩余行（第3行及之后），用于增量更新
	vector<string> remaining_lines;
	ifstream in_file{ m_file_path };
	if (in_file.is_open())
	{
		string line;
		int line_num = 0;
		while (std::getline(in_file, line))
		{
			line_num++;
			if (line_num > 2) // 跳过前两行
			{
				remaining_lines.push_back(line);
			}
		}
		in_file.close();
	}

	// 写入更新后的文件
	ofstream out_file{ m_file_path };
	if (!out_file.is_open())
	{
		return;
	}

	// 第一行：lines计数（今天的记录 + 历史记录）
	size_t total_size = 1 + m_history_traffics.size();
	char buff[64];
	sprintf_s(buff, "lines: \"%u\"", static_cast<unsigned int>(total_size));
	out_file << buff << "\n";

	// 第二行：今天的记录
	WriteTrafficRecord(out_file, m_today_traffic);

	// 剩余行：从文件读取，直接写入（不格式化）
	for (const auto& line : remaining_lines)
	{
		out_file << line << "\n";
	}

	out_file.close();
}

void CHistoryTrafficFile::Load()
{
	m_today_traffic = HistoryTraffic{}; // 初始化今天的记录
	m_history_traffics.clear(); // 清空历史记录链表
	InvalidateCache(); // 标记缓存过期

	ifstream file{ m_file_path };
	string current_line, temp;
	HistoryTraffic traffic;
	bool is_first_data_line = true; // 标记是否是第一条数据行（今天的记录）
	
	if (CCommon::FileExist(m_file_path.c_str()))
	{
		// 跳过第一行（lines:）
		std::getline(file, current_line);
		
		while (!file.eof())
		{
			std::getline(file, current_line);

			if (current_line.size() < 12)
			{
				continue;
			}
			temp = current_line.substr(0, 4);
			traffic.year = atoi(temp.c_str());
			if (traffic.year < 1900 || traffic.year > 3000)
			{
				continue;
			}
			temp = current_line.substr(5, 2);
			traffic.month = atoi(temp.c_str());
			if (traffic.month < 1 || traffic.month > 12)
			{
				continue;
			}
			temp = current_line.substr(8, 2);
			traffic.day = atoi(temp.c_str());
			if (traffic.day < 1 || traffic.day > 31)
			{
				continue;
			}

			size_t separator_index = current_line.find(L'/', 11);
			traffic.mixed = (separator_index == wstring::npos);
			if (traffic.mixed)
			{
				temp = current_line.substr(11);
				traffic.down_kBytes = atoll(temp.c_str());
				traffic.up_kBytes = 0;
			}
			else
			{
				temp = current_line.substr(11, separator_index - 11);
				traffic.up_kBytes = atoll(temp.c_str());
				temp = current_line.substr(separator_index + 1);
				traffic.down_kBytes = atoll(temp.c_str());
			}
			
			if (traffic.year > 0 && traffic.month > 0 && traffic.day > 0 && traffic.kBytes() > 0)
			{
				if (is_first_data_line)
				{
					// 第一条数据行是今天的记录
					m_today_traffic = traffic;
					is_first_data_line = false;
				}
				else
				{
					// 其余是历史记录，插入到链表
					m_history_traffics.push_back(traffic);
				}
			}
		}
	}

	MormalizeData();
}

void CHistoryTrafficFile::LoadSize()
{
	ifstream file{ m_file_path };
	string current_line, temp;
	if (CCommon::FileExist(m_file_path.c_str()))
	{
		std::getline(file, current_line); // 读取第一行
		size_t index = current_line.find("lines:");
		if (index != wstring::npos)
		{
			index = current_line.find("\"", index + 6);
			size_t index1 = current_line.find("\"", index + 1);
			temp = current_line.substr(index + 1, index1 - index - 1);
			m_size = atoll(temp.c_str());
		}
	}
}

void CHistoryTrafficFile::Merge(const CHistoryTrafficFile& history_traffic, bool ignore_same_data)
{
	HistoryTraffic today_traffic = CreateTodayTraffic();

	// 合并今天的记录（只合并日期相同的）
	// 注意：如果 ignore_same_data=true，说明是从备份恢复，应该取较大的值而不是累加，避免重复累加
	if (HistoryTraffic::DateEqual(m_today_traffic, history_traffic.m_today_traffic))
	{
		if (ignore_same_data)
		{
			// 从备份恢复时，取较大的值（避免重复累加）
			// 备份文件通常是程序退出时的完整数据，当前文件可能是程序启动后的不完整数据
			if (history_traffic.m_today_traffic.up_kBytes > m_today_traffic.up_kBytes)
			{
				m_today_traffic.up_kBytes = history_traffic.m_today_traffic.up_kBytes;
			}
			if (history_traffic.m_today_traffic.down_kBytes > m_today_traffic.down_kBytes)
			{
				m_today_traffic.down_kBytes = history_traffic.m_today_traffic.down_kBytes;
			}
		}
		else
		{
			// 正常合并时，累加数据
			m_today_traffic.up_kBytes += history_traffic.m_today_traffic.up_kBytes;
			m_today_traffic.down_kBytes += history_traffic.m_today_traffic.down_kBytes;
		}
	}

	// 合并历史记录链表
	for (const HistoryTraffic& traffic : history_traffic.m_history_traffics)
	{
		// 跳过"未来"的记录（系统时间可能被调整了）
		if (HistoryTraffic::DateGreater(traffic, today_traffic))
		{
			continue; // 跳过"未来"的记录
		}

		if (ignore_same_data)
		{
			// 如果要忽略相同日期的项，使用线性查找（list不支持随机访问）
			auto it = std::find_if(m_history_traffics.begin(), m_history_traffics.end(),
				[&traffic](const HistoryTraffic& existing) {
					return HistoryTraffic::DateEqual(existing, traffic);
				});
			if (it != m_history_traffics.end())
			{
				continue; // 找到相同日期的记录，跳过
			}
		}
		m_history_traffics.push_back(traffic);
	}
	
	MormalizeData();
	InvalidateCache(); // 标记缓存过期
}

void CHistoryTrafficFile::OnDateChanged()
{
	// 日期改变时，将今天的记录移到历史记录链表的前面，然后创建新的今天的记录
	
	// 如果今天的记录有数据，将其移到历史记录链表
	if (m_today_traffic.kBytes() > 0)
	{
		m_history_traffics.push_front(m_today_traffic);
		// 立即排序，确保数据一致性（按日期从大到小）
		if (m_history_traffics.size() >= 2)
		{
			m_history_traffics.sort(HistoryTraffic::DateGreater);
		}
	}
	
	// 创建新的今天的记录
	m_today_traffic = CreateTodayTraffic();
	
	// 更新统计
	m_today_up_traffic = 0;
	m_today_down_traffic = 0;
	m_size = 1 + m_history_traffics.size();
	InvalidateCache(); // 标记缓存过期
}

void CHistoryTrafficFile::MormalizeData()
{
	HistoryTraffic today_traffic = CreateTodayTraffic();

	// 先对历史记录链表排序（按日期从大到小），以便后续查找和合并
	if (m_history_traffics.size() >= 2)
	{
		m_history_traffics.sort(HistoryTraffic::DateGreater);

		// 合并相同日期的记录
		auto it = m_history_traffics.begin();
		while (it != m_history_traffics.end())
		{
			auto next_it = it;
			++next_it;
			if (next_it != m_history_traffics.end() && HistoryTraffic::DateEqual(*it, *next_it))
			{
				it->up_kBytes += next_it->up_kBytes;
				it->down_kBytes += next_it->down_kBytes;
				m_history_traffics.erase(next_it);
			}
			else
			{
				++it;
			}
		}
	}

	// 清理日期晚于当前日期的历史记录（系统时间可能被调整了）
	// 历史记录应该都是过去的日期，不应该有未来的日期
	if (!m_history_traffics.empty())
	{
		auto it = m_history_traffics.begin();
		while (it != m_history_traffics.end())
		{
			// 如果历史记录的日期晚于今天，说明是"未来"的记录，应该删除
			if (HistoryTraffic::DateGreater(*it, today_traffic))
			{
				it = m_history_traffics.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// 如果 m_today_traffic 的日期也晚于当前日期，说明系统时间被调整了，应该重置
	if (HistoryTraffic::DateGreater(m_today_traffic, today_traffic))
	{
		// 如果今天的记录有数据，应该将其移到历史记录（但日期晚于今天，会被上面的清理逻辑删除）
		// 直接重置为今天的记录
		m_today_traffic = today_traffic;
	}

	// 在历史记录中查找今天的记录（可能历史记录中包含了今天的数据）
	auto it = std::find_if(m_history_traffics.begin(), m_history_traffics.end(),
		[&today_traffic](const HistoryTraffic& traffic) {
			return HistoryTraffic::DateEqual(traffic, today_traffic);
		});

	if (it != m_history_traffics.end())
	{
		// 历史记录中找到了今天的记录
		if (HistoryTraffic::DateEqual(m_today_traffic, today_traffic))
		{
			// 如果 m_today_traffic 也是今天的，合并数据（避免数据丢失）
			m_today_traffic.up_kBytes += it->up_kBytes;
			m_today_traffic.down_kBytes += it->down_kBytes;
		}
		else
		{
			// 如果 m_today_traffic 不是今天的，用历史记录中的替换
			m_today_traffic = *it;
		}
		// 从历史记录中删除今天的记录（因为应该只在 m_today_traffic 中）
		m_history_traffics.erase(it);
	}
	else if (!HistoryTraffic::DateEqual(m_today_traffic, today_traffic))
	{
		// 历史记录中没有今天的记录，且 m_today_traffic 也不是今天的
		// 如果 m_today_traffic 有数据，应该将其移到历史记录链表
		if (m_today_traffic.kBytes() > 0)
		{
			m_history_traffics.push_front(m_today_traffic);
			// 重新排序（因为插入了新记录）
			if (m_history_traffics.size() >= 2)
			{
				m_history_traffics.sort(HistoryTraffic::DateGreater);
			}
		}
		// 创建新的今天的记录
		m_today_traffic = today_traffic;
	}

	// 更新今天的流量统计
	m_today_up_traffic = static_cast<__int64>(m_today_traffic.up_kBytes) * 1024;
	m_today_down_traffic = static_cast<__int64>(m_today_traffic.down_kBytes) * 1024;
	m_today_traffic.mixed = false;

	// 更新总记录数
	m_size = 1 + m_history_traffics.size();
	InvalidateCache(); // 标记缓存过期
}
