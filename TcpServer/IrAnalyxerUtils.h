#pragma once

#include <map>
#include <vector>
#include <algorithm>

enum IrCommandT {
	IR_VOLUME_UP = 0,
	IR_VOLUME_DOWN,
	IR_VOLUME_MUTE,
	IR_CMD_COUNT
};

class IrAnalyzerUtils {
public:
	static std::vector<int> analyzeRecordBuffer(const char* p_buff, int len)
	{
		bool store = false;
		std::vector<int> bytes;
		std::vector<std::vector<int>> matrix;
		char c;
		for (int i = 0; i < len; i++) {
			c = p_buff[i];
			if (c == 0xFF) {
				store = true;
				if (bytes.size() >= 4) {
					matrix.push_back(bytes);
				}
				bytes.clear();
			}
			else if (store) {
				bytes.push_back(c);
			}
		}

		std::map<std::vector<int>, int> bytesBinMap;
		std::vector<std::vector<int>>::const_iterator iter = matrix.cbegin();
		while (iter != matrix.cend()) {
			bytesBinMap[(*iter)]++;
			++iter;
		}

		using pair_type = decltype(bytesBinMap)::value_type;
		auto pr = std::max_element
		(
			std::begin(bytesBinMap), std::end(bytesBinMap),
			[](const pair_type& p1, const pair_type& p2) {
				return p1.second < p2.second;
			}
		);

		return pr->first;
	}

	static void getIrCodes(std::vector<int>& p_bytes, std::vector<uint32_t>& p_codes)
	{
		if (p_bytes.size() >= 4) {
			int bytes = 0;
			uint32_t code = 0x0;

			std::vector<int>::const_iterator iter = p_bytes.cbegin();
			while (iter != p_bytes.cend()) {
				code = (code << 8) | (*iter);
				++iter;
				if (++bytes == 4) {
					p_codes.push_back(code);
					break;
				}
			}

			p_bytes.erase(p_bytes.begin());
			getIrCodes(p_bytes, p_codes);
		}
	}

	static std::vector<uint32_t> getUniqueIrCodes(std::vector<uint32_t>& p_uniqueCodes, std::vector<uint32_t> p_codes)
	{
		std::vector<uint32_t> codes;

		std::vector<uint32_t>::const_iterator iter = p_codes.cbegin();
		while (iter != p_codes.cend()) {
			if (std::find(p_uniqueCodes.begin(), p_uniqueCodes.end(), (*iter)) != p_uniqueCodes.end()) {
				codes.push_back(*iter);
			}
			++iter;
		}

		return codes;
	}

	static void getUniqueIrCodesMap(std::map<IrCommandT, std::vector<uint32_t>>& p_uniqueIrCodeMap, std::map<IrCommandT, std::vector<uint32_t>> p_irCodeMap)
	{
		std::map<uint32_t, int> scoredIrCodeMap;
		std::map<IrCommandT, std::vector<uint32_t>>::const_iterator iter = p_irCodeMap.cbegin();
		while (iter != p_irCodeMap.cend()) {
			IrAnalyzerUtils::addScoredIrCodeMap(scoredIrCodeMap, iter->second);
			++iter;
		}

		std::vector<uint32_t> allUniqueIrCodes;
		std::map<uint32_t, int>::const_iterator mapIter = scoredIrCodeMap.cbegin();
		while (mapIter != scoredIrCodeMap.cend()) {
			if ((*mapIter).second == 1) {
				allUniqueIrCodes.push_back((*mapIter).first);
			}
			++mapIter;
		}

		iter = p_irCodeMap.cbegin();
		while (iter != p_irCodeMap.cend()) {
			std::vector<uint32_t> codes = IrAnalyzerUtils::getUniqueIrCodes(allUniqueIrCodes, iter->second);
			p_uniqueIrCodeMap[iter->first] = codes;
			++iter;
		}		
	}

	/*static uint32_t getUniqueCode(std::vector<uint32_t> p_mappedIrCodes, std::vector<uint32_t> p_irCodes)
	{
		uint32_t irCode = 0x0;

		std::vector<uint32_t>::const_reverse_iterator iter = p_irCodes.crbegin();
		while (iter != p_irCodes.crend()) {
			if (std::find(p_mappedIrCodes.begin(), p_mappedIrCodes.end(), (*iter)) == p_mappedIrCodes.end()) {
				irCode = (*iter);
				break;
			}
			++iter;
		}

		return irCode;
	}*/

	static uint32_t getIrBinCode(std::vector<uint32_t> p_irCodes)
	{
		int max = 0;
		uint32_t irCode = 0x0;
		std::map<uint32_t, int> irNibbleMap;

		std::vector<uint32_t>::const_iterator iter = p_irCodes.begin();
		while (iter != p_irCodes.cend()) {
			for (int i = 0; i < 31; i += 4) {
				int code = ((*iter) >> i) & 0xF;
				//printf("%X ", code);
				if (code == 0xB) {
					irNibbleMap[(*iter)] += 1;
				}
				/*else if (code == 0x3) {
					irNibbleMap[(*iter)] += 2;
				}
				else if (code == 0x8) {
					irNibbleMap[(*iter)]++;
				}
				else if (code == 0x4) {
					irNibbleMap[(*iter)]++;
				}
				else if (code == 0x0) {
					irNibbleMap[(*iter)]--;
				}*/
			}
			if (irNibbleMap[(*iter)] > max) {
				irCode = (*iter);
				max = irNibbleMap[(*iter)];
			}

			++iter;
			//printf("\n");
		}

		/*std::map<uint32_t, int>::const_iterator mapIter = irCodesMap.cbegin();
		while (mapIter != irCodesMap.cend()) {
			if (mapIter->second > max) {
				irCode = mapIter->first;
				max = mapIter->second;
			}
			printf("0x%08X: %d\n", mapIter->first, mapIter->second);
			++mapIter;
		}*/

		return irCode;
	}

	/*static void trimCommand(std::vector<std::vector<int>>& p_matrix)
	{
		std::map<int, int> sizeMap;

		std::vector<std::vector<int>>::const_iterator iter = p_matrix.begin();
		std::vector<std::vector<int>>::const_iterator iterEnd = p_matrix.end();
		while (iter != iterEnd) {
			sizeMap[(*iter).size()];
			++iter;
		}

		if (sizeMap.size() != 1) {
			return;
		}
	}*/

	/*static bool containsCode(uint32_t p_code, std::vector<uint32_t> p_codes)
	{
		std::vector<uint32_t>::const_iterator iter = p_codes.begin();
		std::vector<uint32_t>::const_iterator iterEnd = p_codes.end();
		while (iter != iterEnd) {
			if ((*iter) == p_code) {
				return true;
			}
			++iter;
		}

		return false;
	}*/

	static void addScoredIrCodeMap(std::map<uint32_t, int>& p_map, std::vector<uint32_t> p_codes)
	{
		std::vector<uint32_t>::const_iterator iter = p_codes.begin();
		std::vector<uint32_t>::const_iterator iterEnd = p_codes.end();
		while (iter != iterEnd) {
			p_map[*iter]++;
			++iter;
		}
	}

	static void trimStart(std::vector<std::vector<int>>& p_matrix)
	{
		int minLength = -1;

		std::vector<std::vector<int>>::iterator iter = p_matrix.begin();
		std::vector<std::vector<int>>::iterator iterEnd = p_matrix.end();
		while (iter != iterEnd) {
			if ((minLength == -1) || ((*iter).size() < minLength)) {
				minLength = (*iter).size();
			}
			++iter;
		}

		std::vector<int> copyIndexes;
		for (int i = 0; i < minLength; i++) {
			std::map<int, int> byteBinMap;

			iter = p_matrix.begin();
			iterEnd = p_matrix.end();
			while (iter != iterEnd) {
				byteBinMap[(*iter)[i]];
				++iter;
			}

			if (byteBinMap.size() > 1) {
				iter = p_matrix.begin();
				iterEnd = p_matrix.end();
				while (iter != iterEnd) {
					if (i > 0) {
						int index = i - 1;
						(*iter).erase((*iter).begin(), (*iter).begin() + index);
					}
					++iter;
				}
				break;
			}
		}
	}

	/*static void reverseMatrix(std::vector<std::vector<int>>& p_matrix)
	{
		std::vector<std::vector<int>>::iterator iter = p_matrix.begin();
		std::vector<std::vector<int>>::iterator iterEnd = p_matrix.end();
		while (iter != iterEnd) {
			std::reverse((*iter).begin(), (*iter).end());
			++iter;
		}
	}

	static void trimEnd(std::vector<std::vector<int>>& p_matrix)
	{
		reverseMatrix(p_matrix);
		trimStart(p_matrix);
		reverseMatrix(p_matrix);
	}

	static void trim(std::vector<std::vector<int>>& p_matrix)
	{
		trimStart(p_matrix);
		trimEnd(p_matrix);
	}*/

	static void logBytes(std::vector<int> p_bytes)
	{
		std::vector<int>::const_iterator iter = p_bytes.cbegin();
		while (iter != p_bytes.cend()) {
			printf("%02X", (*iter));
			++iter;
		}
		printf("\n");
	}

	static void logMatrix(std::vector<std::vector<int>> p_matrix)
	{
		std::vector<std::vector<int>>::const_iterator iter = p_matrix.cbegin();
		while (iter != p_matrix.cend()) {
			logBytes(*iter);
			++iter;
		}
	}

	static void logIrCodes(std::vector<uint32_t> p_codes)
	{
		std::vector<uint32_t>::const_iterator iter = p_codes.cbegin();
		while (iter != p_codes.cend()) {
			printf("%08X ", (*iter));
			++iter;
		}
		printf("\n");
	}

	static void logIrCodeMatrix(std::vector<std::vector<uint32_t>> p_matrix)
	{
		std::vector<std::vector<uint32_t>>::const_iterator iter = p_matrix.cbegin();
		while (iter != p_matrix.cend()) {
			logIrCodes(*iter);
			++iter;
		}
	}

	static void logIrCodeMap(std::map<IrCommandT, std::vector<uint32_t>> p_map)
	{
		std::map<IrCommandT, std::vector<uint32_t>>::const_iterator iter = p_map.cbegin();
		while (iter != p_map.cend()) {
			logIrCodes(iter->second);
			++iter;
		}
	}
};
