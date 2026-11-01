# notebooks

알고리즘을 라이브러리에 넣기 전에 개념을 Jupyter 노트북으로 공부하는 곳. 가상환경은 uv로 관리한다.

```sh
cd demo/notebooks
uv sync                 # .venv 만들고 패키지 설치
uv run jupyter lab      # 브라우저에서 열기
```

VS Code에서 열 때는 커널로 `demo/notebooks/.venv`의 Python을 고른다.

외삽 개념부터 공부하고(`01_concepts/`), 그다음 같은 내용을 배 위치에 쓰는 해양 응용(`02_applications/`)으로 넘어간다.

### 01_concepts: 개념 (항해와 상관없는 예시)

| 노트북 | 내용 |
|---|---|
| `01_extrapolation.ipynb` | 외삽이란, 선형 외삽 공식, 배터리·택시 요금 예시, 일상에서 쓰이는 곳(다운로드 남은 시간, 터널 속 내 위치 등) |
| `02_extrapolation_limits.ipynb` | 외삽이 틀리는 경우와 보완: 멀리 갈수록(키), 잡음(기온, 여러 점으로), 곡선(제동, 2차식), 고차식의 위험 |
| `03_kalman_filter.ipynb` | 칼만 필터: 예측(= 선형 외삽) + 보정, 재지 않은 속도 추정 |
| `04_extended_kalman_filter.ipynb` | 확장 칼만 필터: 비선형 움직임, 야코비안, 원을 도는 물체의 도는 속도 추정, 곡선 외삽 |

### 02_applications: 해양 응용 (AIS 보고를 받는 배)

| 노트북 | 내용 | 개념 |
|---|---|---|
| `01_dead_reckoning.ipynb` | 직선 외삽(직선 DR): SOG·COG를 북·동으로 나누기, 위도·경도 변환, 잡음과 선회에서 틀리는 모습 | 개념 1·2 |
| `02_kalman_filter.ipynb` | 칼만 필터로 다듬은 위치·속도로 직선 외삽 | 개념 3 |
| `03_extended_kalman_filter.ipynb` | 확장 칼만 필터로 도는 속도까지 추정해 곡선 외삽, 세 방법 비교 | 개념 4 |

`02_applications/sim.py`는 해양 응용 노트북이 함께 쓰는 가짜 배 시뮬레이션(정답 항적, 잡음 섞인 AIS 보고, 오차 채점)이다.
