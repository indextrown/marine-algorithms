#ifndef MARINE_C_H
#define MARINE_C_H

/*
 * marine-algorithms C API.
 *
 * 모든 언어 바인딩(Swift, Kotlin/JNI, WASM)이 이 헤더 하나만 본다.
 * 규칙
 *   - 함수 이름은 ma_ 접두사.
 *   - C++ 예외는 이 경계를 넘지 않는다. 실패할 수 있는 함수는 ma_status를 돌려주고
 *     결과는 out 포인터에 쓴다. 실패하면 out은 건드리지 않는다.
 *   - 상태가 필요한 객체는 불투명 핸들 + ma_*_new / ma_*_free 짝으로 노출한다.
 */

#ifdef __cplusplus
extern "C" {
#endif

/// 실패할 수 있는 함수의 결과.
typedef enum ma_status {
    MA_OK = 0,
    /// 반복 계산이 수렴하지 않았다(Vincenty 역문제의 대척점 근처).
    MA_NOT_CONVERGED = 1,
    /// 인자가 잘못됐다. out 포인터가 NULL이거나, 위도가 [-90, 90] 밖이거나,
    /// 위도·경도·방위·거리가 NaN·무한대이거나, 타원체가 장반경 > 0, 편평률 [0, 1)을 벗어났다.
    MA_INVALID_ARGUMENT = 2,
} ma_status;

/// 위도·경도(도 단위).
typedef struct ma_coordinate {
    double latitude;
    double longitude;
} ma_coordinate;

/// 회전 타원체. 장반경 a(m)와 편평률 f.
typedef struct ma_ellipsoid {
    double semi_major_axis;
    double flattening;
} ma_ellipsoid;

/// 역문제의 답. 방위는 진북 기준 시계 방향 [0, 360).
typedef struct ma_inverse {
    double distance;        /* m */
    double initial_bearing; /* 출발점에서의 방위 */
    double final_bearing;   /* 도착점에서의 방위(진행 방향) */
} ma_inverse;

/// 정문제의 답.
typedef struct ma_direct {
    ma_coordinate destination;
    double final_bearing; /* 도착점에서의 방위(진행 방향) [0, 360) */
} ma_direct;

/// GPS가 쓰는 WGS84 타원체.
ma_ellipsoid ma_wgs84(void);

/* 단위 변환. 해리 = 1852m, 노트 = 1해리/시 */

double ma_meters_to_nautical_miles(double meters);
double ma_nautical_miles_to_meters(double nautical_miles);
double ma_knots_to_meters_per_second(double knots);
double ma_meters_per_second_to_knots(double meters_per_second);

/* 구면 (하버사인) */

/// 대권 거리(m). 하버사인 공식. 잘못된 좌표면 NaN.
double ma_haversine_distance(ma_coordinate from, ma_coordinate to);

/// 처음 방위(도, [0, 360)). 잘못된 좌표면 NaN.
double ma_initial_bearing(ma_coordinate from, ma_coordinate to);

/* 타원체 (Vincenty 1975). 대척점 근처에서 MA_NOT_CONVERGED, 잘못된 입력이면 MA_INVALID_ARGUMENT. */

ma_status ma_vincenty_inverse(ma_coordinate from, ma_coordinate to, ma_ellipsoid ellipsoid,
                              ma_inverse* out);

ma_status ma_vincenty_direct(ma_coordinate from, double bearing, double distance,
                             ma_ellipsoid ellipsoid, ma_direct* out);

#ifdef __cplusplus
}
#endif

#endif /* MARINE_C_H */
