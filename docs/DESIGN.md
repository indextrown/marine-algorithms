# 설계 규칙

## 층

```
C++23 코어      include/marine, src/          namespace marine::<모듈>
   ↓
C 다리층        bindings/c/                   extern "C", ma_ 접두사
   ↓
언어 바인딩     bindings/swift/ (이후 kotlin/, wasm/)
```

언어 바인딩은 C 헤더 `marine_c.h`만 본다. C++ 헤더를 바인딩에 노출하지 않는다.

## C++ 코어

- 표준은 C++23. Android NDK r26 이상(clang 17, libc++ 17)과 Swift 패키지 양쪽에서 빌드된다. 코어 안의 실패는 `std::expected`로 돌려준다.
- 외부 의존성 없음(테스트용 Catch2 제외).
- 단위는 SI 기본: 거리 m, 시간 s, 속도 m/s. 각도는 공개 API에서 도(°), 내부 계산은 라디안.
- 순수 계산은 `noexcept` 자유 함수로. 상태가 필요한 것(필터 등)만 클래스로.

## C 다리층

| 규칙 | 이유 |
|---|---|
| 값으로 넘길 수 있는 건 POD 구조체(`ma_coordinate`) | 수명 관리가 필요 없다 |
| 상태 있는 객체는 불투명 핸들 + `ma_*_new` / `ma_*_free` | C++ 타입을 숨기고 소유를 명확히 한다 |
| 예외를 넘기지 않는다. 실패할 수 있는 함수는 `ma_status`를 돌려주고 결과는 out 포인터에 쓴다. 실패하면 out은 건드리지 않는다 | C·JNI·WASM이 C++ 예외를 받을 수 없다 |
| 실패하지 않는 함수는 결과를 값으로 돌려준다(`ma_haversine_distance`) | 쓰는 쪽이 상태를 확인할 필요가 없다 |
| 콜백을 두지 않는다(pull 방식) | 거꾸로 넘는 콜백은 바인딩마다 가장 까다롭다 |

## Swift 바인딩

- C 값은 Swift 값 타입으로 바꿔 돌려준다. 네이티브 자원을 쥔 타입만 `class` + `deinit`에서 `ma_*_free`.
- 오류 흐름: C++ `std::expected<T, E>` → C `ma_status` + out → Swift `throws(E)`(타입 있는 throws).
- 테스트는 Swift Testing(`@Test`).

## 테스트

- C++: 알고리즘 정확도는 C++ 테스트에서 본다. 기준값은 공개 자료(GeographicLib 등)에서 가져온다.
- C API: 코어와 같은 값을 돌려주는지만 본다.
- Swift: 바인딩이 제대로 이어졌는지만 본다.
