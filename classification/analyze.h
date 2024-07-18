#include "../commons/data_formats.h"

typedef enum ActionType {
	SIT,
	STAND,
	OTHER,
	LENGTH
} ActionType;

typedef struct AnalysisResult {
	ActionType classification;
} AnalysisResult;

AnalysisResult analyze(BMI2SensData data[]);

