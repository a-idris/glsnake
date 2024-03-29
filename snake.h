#ifndef SNAKE_H
#define SNAKE_H

enum perspective_t { ORTHOGRAPHIC=0, PERSPECTIVE=1, POV=2, PERSPECTIVES_COUNT=3 };

struct material_t {
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
};

#endif