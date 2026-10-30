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
    /// out 포인터가 NULL이다.
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

/* 구면 (하버사인) */

/// 대권 거리(m). 하버사인 공식.
double ma_haversine_distance(ma_coordinate from, ma_coordinate to);

/// 처음 방위(도, [0, 360)).
double ma_initial_bearing(ma_coordinate from, ma_coordinate to);

/* 타원체 (Vincenty 1975). 대척점 근처에서 MA_NOT_CONVERGED. */

ma_status ma_vincenty_inverse(ma_coordinate from, ma_coordinate to, ma_ellipsoid ellipsoid,
                              ma_inverse* out);

ma_status ma_vincenty_direct(ma_coordinate from, double bearing, double distance,
                             ma_ellipsoid ellipsoid, ma_direct* out);

#ifdef __cplusplus
}
#endif

#endif /* MARINE_C_H */
