#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include "Vector3.h"

enum class MapChipType { kBlank, kBlock };

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
	public:
		void ResteMapChipData();
	    void LoadMapChipCsv(const std::string& filePath);
		MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
		Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	    uint32_t GetNunBlockHorizontal();
	    uint32_t GetNunBlockVirtical();
	private:
		static inline const float kBlokeWidth = 2.0f;
		static inline const float kBlockHeighth = 2.0f;
	    static inline const uint32_t kNumBlockVirtical = 20;
	    static inline const uint32_t kNumBlockHorizontal = 100;

		MapChipData mapChipData_;
};



