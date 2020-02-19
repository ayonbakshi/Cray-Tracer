/***********************************************************************************
	Created:	17:9:2002
	FileName: 	hdrloader.h
	Author:		Igor Kravtchenko
    Edits made by Ayon Bakshi
	
	Info:		Load HDR image and convert to a set of float32 RGB triplet.
************************************************************************************/
#pragma once

#include <cmath>

#include "MathUtils.h"

class HDRI {
public:
	int width, height;
    double theta = 0; // rotation on y axis
	// each pixel takes 3 float32, each component can be of any value...
	float *cols;
    
    HDRI(): cols{nullptr} {}
    ~HDRI() { delete[] cols; }

    Vec3d get_pixel(Vec3d dir) const {

        if (theta != 0) {
             // rotate
             double mag = sqrt(dir[0] * dir[0] + dir[2] * dir[2]);
             double angle = atan2(dir[2], dir[0]) + theta;

             dir[0] = mag * cos(angle);
             dir[2] = mag * sin(angle);
             dir.normalize();
        }

        // spherical projection
        double u = 0.5 + atan2(dir[2], dir[0]) * M_1_PI * 0.5;
        double v = 0.5 - asin(dir[1]) * M_1_PI;
        int x = u * width, y = v * height;
        int idx = 3 * (y * width + x);
        
        // tone map?
        Vec3d ret;
        for (int i = 0; i < 3; ++i) {
            ret[i] = cols[idx + i];

            ret[i] = gamma_compression(ret[i], 0.6, 0.8);
            // ret[i] = contrast_tone_map(ret[i]);
        }
        ret.clamp(0, 2);
        return ret;
    }
};

class HDRLoader {
public:
	static bool load(const char *fileName, HDRI &res);
};