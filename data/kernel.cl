__constant int ObjectTypePlane = 0;
__constant int ObjectTypeSphere = 1;
__constant int ObjectTypeBox = 2;
__constant float kMaxRenderDist = 1e20;
__constant float kPixelXRes = 4.f;
__constant float kPixelYRes = 4.f;
__constant bool kAntiAliasOn = false;
__constant float kEpsilon = 0.0001f;

struct PointLight {
	float4 position;
	float4 color;
};

struct Ray {
	float4 origin;
	float4 dir;
};

struct Material {
	float4 color;
	float specpower;
	float speccoefficient;
};

struct Plane {
	float4 pointonplane;
	float4 normal;
	int objecttype;
	struct Material material;
};

struct Sphere {
	float radius;
	float4 pos;
	int objecttype;
	struct Material material;
};

struct Box {
	float4 minimum;
	float4 maximum;
	int objecttype;
	struct Material material;
};

struct IntersectionInfo {
	bool bIsIntersecting;
	float fT;
	float4 vIntersectionPoint;
	float4 vNormal;
};

struct Scene {
	struct Sphere spheres[2];
	int spherescount;
	struct Plane planes[2];
	int planescount;
	struct Box boxes[1];
	int boxescount;
};

float4 matrixvectormul(float4 col1, float4 col2, float4 col3, float4 col4, float4 vec) {
	return (float4)(dot(col1, vec), dot(col2, vec), dot(col3, vec), dot(col4, vec));
}

struct IntersectionInfo CreateIntersectionInfo() {
    struct IntersectionInfo result;
	result.bIsIntersecting = false;
	result.fT = kMaxRenderDist;
	result.vIntersectionPoint = (float4)(0.f, 0.f, 0.f, 0.f);
	result.vNormal = (float4)(0.f, 0.f, 0.f, 0.f);
    return result;
}

struct Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height, const float pixelXOffset, const float pixelYOffset, const float4 cameraPos, const float4 lookAt) {
	float aspectratio = (float)(width) / (float)(height);
	float x = (2.f * (x_coord + pixelXOffset) / width - 1.f) * aspectratio * tan(radians(25.f));
    float y = (1.f - 2.f * (y_coord + pixelYOffset) / height) * tan(radians(25.f)); 
	float planedistancefromeyepos = 1.f;
	float4 up = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
	float4 w = lookAt;
	float4 u = normalize(cross(up, w));
	float4 v = normalize(cross(w, u));
	float4 tmpx = u * x;
	float4 tmpy = v * y;
	float4 tmpw = w * planedistancefromeyepos;
	float4 tobeadded = tmpx + tmpy;
	float4 dir = normalize(tobeadded - tmpw);
	struct Ray ray;
	ray.origin = cameraPos;
	ray.dir = dir;
	return ray;
}

struct IntersectionInfo intersect_plane(const struct Plane* plane, const struct Ray* ray) {
	struct IntersectionInfo result = CreateIntersectionInfo();
	float denom = dot(plane->normal, ray->dir); 
    if (denom > -1e6) {
        float4 fromeyetoplane = plane->pointonplane - ray->origin; 
        result.fT = dot(fromeyetoplane, plane->normal) / denom;
		if (result.fT >= 0.f) {
			result.vNormal = plane->normal;
			result.vIntersectionPoint = ray->origin + (result.fT * ray->dir);
		}
		result.bIsIntersecting = (result.fT >= 0.f);
        return result; 
    }
	return result;
}

struct IntersectionInfo intersect_sphere(const struct Sphere* sphere, const struct Ray* ray) {
	struct IntersectionInfo result = CreateIntersectionInfo();
	float4 rayToCenter = sphere->pos - ray->origin;
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c;
	if (disc < 0.0f) {
		return result;
	} else {
		result.fT = b - sqrt(disc);
	}
	if (result.fT < 0.0f) {
		result.fT = b + sqrt(disc);
		if (result.fT < 0.0f) {
			return result;
		}
	} else {
		result.vIntersectionPoint = ray->origin + (result.fT * ray->dir);
		result.vNormal = normalize(result.vIntersectionPoint - sphere->pos);
		result.bIsIntersecting = true;
		return result;
	}
	return result;
}

void getBoxNearFar(const float* boxMinComponent, const float* boxMaxComponent, const float* rayOriginComponent, const float* rayDirComponent, float* tNear, float* tFar) {
	float tComponent1 = (*boxMinComponent - *rayOriginComponent) / *rayDirComponent;
	float tComponent2 = (*boxMaxComponent - *rayOriginComponent) / *rayDirComponent;
	if (tComponent1 > tComponent2) {
		float tmp = tComponent1;
		tComponent1 = tComponent2;
		tComponent2 = tmp;
	}
	if (tComponent1 > *tNear) {
		*tNear  = tComponent1;
	}
	if (tComponent2 < *tFar) {
		*tFar = tComponent2;
	}
}

struct IntersectionInfo intersect_box(const struct Box* box, const struct Ray* ray) {
	struct IntersectionInfo result = CreateIntersectionInfo();
	float tNear = FLT_MIN;
	float tFar = FLT_MAX;
	if (ray->dir.x == 0.0f) {
		if (ray->origin.x < box->minimum.x || ray->origin.x > box->maximum.x) {
			return result;
		}
	} else {
		float f1 = box->minimum[0];
		float f2 = box->maximum[0];
		float f3 = ray->origin[0];
		float f4 = ray->dir[0];
		getBoxNearFar(&f1, &f2, &f3, &f4, &tNear, &tFar);
		if (tNear > tFar || tFar < 0.0f) {
			return result;
		}
	}
	if (ray->dir.y == 0.0f) {
		if (ray->origin.y < box->minimum.y || ray->origin.y > box->maximum.y) {
			return result;
		}
	} else {
		float f1 = box->minimum[1];
		float f2 = box->maximum[1];
		float f3 = ray->origin[1];
		float f4 = ray->dir[1];
		getBoxNearFar(&f1, &f2, &f3, &f4, &tNear, &tFar);
		if (tNear > tFar || tFar < 0.0f) {
			return result;
		}
	}
	if (ray->dir.z == 0.0f) {
		if (ray->origin.z < box->minimum.z || ray->origin.z > box->maximum.z) {
			return result;
		}
	} else {
		float f1 = box->minimum[2];
		float f2 = box->maximum[2];
		float f3 = ray->origin[2];
		float f4 = ray->dir[2];
		getBoxNearFar(&f1, &f2, &f3, &f4, &tNear, &tFar);
		if (tNear > tFar || tFar < 0.0f) {
			return result;
		}
	}
	result.fT = tNear;
	result.vIntersectionPoint = ray->origin + (result.fT * ray->dir);
	if (fabs(result.vIntersectionPoint.x - box->minimum.x) < kEpsilon) {
		result.vNormal = (float4)(-1.0f, 0.0f, 0.0f, 0.f);
	} else if (fabs(result.vIntersectionPoint.x - box->maximum.x) < kEpsilon) {
		result.vNormal = (float4)(1.0f, 0.0f, 0.0f, 0.f);
	} else if (fabs(result.vIntersectionPoint.y - box->minimum.y) < kEpsilon) {
		result.vNormal = (float4)(0.0f, -1.0f, 0.0f, 0.f);
	} else if (fabs(result.vIntersectionPoint.y - box->maximum.y) < kEpsilon) {
		result.vNormal = (float4)(0.0f, 1.0f, 0.0f, 0.f);
	} else if (fabs(result.vIntersectionPoint.z - box->minimum.z) < kEpsilon) {
		result.vNormal = (float4)(0.0f, 0.0f, 1.0f, 0.f);
	} else if (fabs(result.vIntersectionPoint.z - box->maximum.z) < kEpsilon) {
		result.vNormal = (float4)(0.0f, 0.0f, -1.0f, 0.f);
	}
	result.bIsIntersecting = true;
	return result;
}

float4 reflect(const float4 primaryRayDir, const float4 normal) {
	return normalize(primaryRayDir - ((2.0f * dot(primaryRayDir, normal)) * normal));
}

float4 lambertiancolor(const struct Ray* ray, const struct PointLight* light, const struct Material* material, const float4 normal, const float4 intersectionpoint) {
	float4 toeye = normalize(-ray->dir);
	float4 lightvector = normalize(intersectionpoint - light->position);
	float4 reflection = reflect(lightvector, normal);
	float specfactor = pow(max(dot(reflection, toeye), 0.f), material->specpower);
	float difffactor = max(dot(-lightvector, normal), 0.f);
	float4 spec = specfactor * light->color * material->speccoefficient;
	float4 diff = difffactor * (material->color * light->color);
	float4 amb = (material->color * light->color) * 0.2f;
	return amb + diff + spec;
}

struct IntersectionInfo intersect(const struct Ray* ray, struct Scene* scene, void** object, int* intersectObjType, struct IntersectionInfo* info) {
	struct IntersectionInfo result = CreateIntersectionInfo();;
	float minT = kMaxRenderDist;
	for (int i = 0; i < scene->spherescount; i++) {
		result = intersect_sphere(&scene->spheres[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT < minT) {
				minT = result.fT;
				info->bIsIntersecting = result.bIsIntersecting;
				info->fT = result.fT;
				info->vIntersectionPoint = result.vIntersectionPoint;
				info->vNormal = result.vNormal;
				*intersectObjType = scene->spheres[i].objecttype;
				*object = &scene->spheres[i];
			}
		}
	}
	for (int i = 0; i < scene->planescount; i++) {
		result = intersect_plane(&scene->planes[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT < minT) {
				minT = result.fT;
				info->bIsIntersecting = result.bIsIntersecting;
				info->fT = result.fT;
				info->vIntersectionPoint = result.vIntersectionPoint;
				info->vNormal = result.vNormal;
				*intersectObjType = scene->planes[i].objecttype;
				*object = &scene->planes[i];
			}
		}
	}
	for (int i = 0; i < scene->boxescount; i++) {
		result = intersect_box(&scene->boxes[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT < minT) {
				minT = result.fT;
				info->bIsIntersecting = result.bIsIntersecting;
				info->fT = result.fT;
				info->vIntersectionPoint = result.vIntersectionPoint;
				info->vNormal = result.vNormal;
				*intersectObjType = scene->boxes[i].objecttype;
				*object = &scene->boxes[i];
			}
		}
	}
	return result;
}

float intersectShadow(struct Ray* ray, const struct Scene* scene) {
	float minT = kMaxRenderDist;
	struct IntersectionInfo result = CreateIntersectionInfo();;
	for (int i = 0; i < scene->spherescount; i++) {
		result = intersect_sphere(&scene->spheres[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT < minT) {
				minT = result.fT;
			}
		}
	}
	for (int i = 0; i < scene->planescount; i++) {
		result = intersect_plane(&scene->planes[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT < minT) {
				minT = result.fT;
			}
		}
	}
	for (int i = 0; i < scene->boxescount; i++) {
		result = intersect_box(&scene->boxes[i], ray);
		if (result.bIsIntersecting) {
			if (result.fT + (kEpsilon * 5.f) < minT) {
				minT = result.fT;
			}
		}
	}
	return minT;
}

float shadow(const struct Scene* scene, const struct PointLight* light, float4 hitpoint, float4 normal) {
	float4 shadowRayDir = normalize(light->position - hitpoint);
	float4 shadowRayOrig = hitpoint + (kEpsilon * normal);
	struct Ray shadowRay;
	shadowRay.origin = shadowRayOrig;
	shadowRay.dir = shadowRayDir;
	float t = intersectShadow(&shadowRay, scene);
	if (t < kMaxRenderDist) {
		return 0.2f;
	}
	return 1.f;
}

float4 traceReflected(const struct Ray* ray, struct Scene* scene, const struct PointLight* light){
	void* intersectObj = 0;
	int intersectObjType = 0;
	struct IntersectionInfo info = CreateIntersectionInfo();
	intersect(ray, scene, &intersectObj, &intersectObjType, &info);
	
	float4 color = (float4)(0.f, 0.f, 0.f, 0.f);
	if (info.fT < kMaxRenderDist) {
		float4 hitpoint = ray->origin + ((info.fT + kEpsilon) * ray->dir);
		if (intersectObjType == ObjectTypePlane) {
			color = lambertiancolor(ray, light, &((struct Plane*)intersectObj)->material, info.vNormal, hitpoint);
		}
		if (intersectObjType == ObjectTypeSphere) {
			color = lambertiancolor(ray, light, &((struct Sphere*)intersectObj)->material, info.vNormal, hitpoint);
		}
		if (intersectObjType == ObjectTypeBox) {		
			color = lambertiancolor(ray, light, &((struct Box*)intersectObj)->material, info.vNormal, hitpoint);
		}
		float shadowfactor = shadow(scene, light, info.vIntersectionPoint, info.vNormal);
		color *= shadowfactor;
	}
	return clamp(color, 0.f, 1.f);
}

float4 trace(const struct Ray* ray, struct Scene* scene, const struct PointLight* light) {
	void* intersectObj = 0;
	int intersectObjType = 0;
	struct IntersectionInfo info = CreateIntersectionInfo();
	intersect(ray, scene, &intersectObj, &intersectObjType, &info);
	
	float4 color = (float4)(0.f, 0.f, 0.f, 0.f);
	if (info.fT < kMaxRenderDist) {
		if (intersectObjType == ObjectTypePlane) {
			color = lambertiancolor(ray, light, &((struct Plane*)intersectObj)->material, info.vNormal, info.vIntersectionPoint);
		}
		if (intersectObjType == ObjectTypeSphere) {
			color = lambertiancolor(ray, light, &((struct Sphere*)intersectObj)->material, info.vNormal, info.vIntersectionPoint);
		}
		if (intersectObjType == ObjectTypeBox) {		
			color = lambertiancolor(ray, light, &((struct Box*)intersectObj)->material, info.vNormal, info.vIntersectionPoint);
		}
		struct Ray secondary;
		secondary.origin = info.vIntersectionPoint + (kEpsilon * info.vNormal);
		secondary.dir = reflect(ray->dir, info.vNormal);
		color += traceReflected(&secondary, scene, light);
		float shadowfactor = shadow(scene, light, info.vIntersectionPoint, info.vNormal);
		color *= shadowfactor;
	}
	return clamp(color, 0.f, 1.f);
};

__kernel void render_kernel(__global float4* output, int width, int height, float4 cameraPos, float4 lookAt) {
	const int work_item_id = get_global_id(0);
	int x_coord = work_item_id % width;
	int y_coord = work_item_id / width;
	struct PointLight light;
	light.position = (float4)(-10.0f, 100.0f, 0.0f, 1.f);
	light.color = (float4)(1.0f, 1.0f, 1.0f, 0.f);
	struct Scene scene;
	scene.spheres[0].radius = 2.5f;
	scene.spheres[0].pos = (float4)(-5.f, 0.f, 0.f, 1.f);
	scene.spheres[0].objecttype = ObjectTypeSphere;
	scene.spheres[0].material.color = (float4)(1.f, 0.0f, 0.0f, 0.f);
	scene.spheres[0].material.specpower = 20.f;
	scene.spheres[0].material.speccoefficient = 0.6f;
	scene.spheres[1].radius = 2.5f;
	scene.spheres[1].pos = (float4)(5.f, 0.f, 0.f, 1.f);
	scene.spheres[1].objecttype = ObjectTypeSphere;
	scene.spheres[1].material.color = (float4)(0.f, 0.0f, 1.0f, 0.f);
	scene.spheres[1].material.specpower = 20.f;
	scene.spheres[1].material.speccoefficient = 0.6f;
	scene.spherescount = 2;
	scene.planes[0].pointonplane = (float4)(0.f, -3.f, 0.f, 1.f);
	scene.planes[0].normal = (float4)(0.f, 1.f, 0.f, 0.f);
	scene.planes[0].objecttype = ObjectTypePlane;
	scene.planes[0].material.color = (float4)(0.f, 0.0f, 0.8f, 0.f);
	scene.planes[0].material.specpower = 10.f;
	scene.planes[0].material.speccoefficient = 0.4f;
	scene.planescount = 1;
	scene.boxes[0].minimum = (float4)(-10.f, -2.f, -15.f, 1.f);
	scene.boxes[0].maximum = (float4)(10.f, 10.f, -20.f, 1.f);
	scene.boxes[0].objecttype = ObjectTypeBox;
	scene.boxes[0].material.color = (float4)(0.f, 1.0f, 0.f, 0.f);
	scene.boxes[0].material.specpower = 5.f;
	scene.boxes[0].material.speccoefficient = 0.4f;
	scene.boxescount = 1;
	if (kAntiAliasOn) {
		float tmpPixelX = 1.0f / kPixelXRes;
		float tmpPixelY = 1.0f / kPixelYRes;
		float tmpDiv = kPixelXRes * kPixelYRes;
		float invTmpDiv = 1.0f / tmpDiv;
		float4 tmpColor = (float4)(0.f, 0.f, 0.f, 1.f);
		float red = 0.0f;
		float green = 0.0f;
		float blue = 0.0f;
		for (float offY = 0.f; offY < kPixelYRes; ++offY) {
			for (float offX = 0.f; offX < kPixelXRes; ++offX) {
				float x2 = 0.2f + (offX * tmpPixelX);
				float y2 = 0.4f + (offY * tmpPixelY);
				struct Ray camray = createCamRay(x_coord, y_coord, width, height, x2, y2, cameraPos, lookAt);
				tmpColor = trace(&camray, &scene, &light/*, &info*/);
				red += tmpColor.x;
				green += tmpColor.y;
				blue += tmpColor.z;
			}
		}
		tmpColor = (float4)(red * invTmpDiv, green * invTmpDiv, blue * invTmpDiv, 1.f);
		output[work_item_id] = clamp(tmpColor, 0.f, 1.f);
	} else {
		struct Ray camray = createCamRay(x_coord, y_coord, width, height, 0.5f, 0.5f, cameraPos, lookAt);
		output[work_item_id] = trace(&camray, &scene, &light/*, &info*/);
	}
}