#include "../commons/data_formats.h"
#include "classify.h"
#include "../commons/vec3d.h"

typedef struct AnalysisResult {
	ActionType classification;
} AnalysisResult;

typedef enum AnalysisStatus {
	ANALYSIS_OK
} AnalysisStatus;

/* Analyze and classify motion data. */
AnalysisStatus analyze(
		BMI2SensData data[], 		/* Raw data. */
		uint16_t data_length, 		/* Length of data. */
		BMI2SensData g, 			/* Stable gravity recording. */
		AnalysisResult *out);		/* Output. */

