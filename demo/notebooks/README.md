# notebooks

알고리즘을 라이브러리에 넣기 전에 개념을 Jupyter 노트북으로 공부하는 곳. 가상환경은 uv로 관리한다.

```sh
cd demo/notebooks
uv sync                 # .venv 만들고 패키지 설치
uv run jupyter lab      # 브라우저에서 열기
```

VS Code에서 열 때는 커널로 `demo/notebooks/.venv`의 Python을 고른다.

| 노트북 | 내용 |
|---|---|
| `01_extrapolation.ipynb` | 외삽과 보간의 차이, 외삽이 시간이 지날수록 틀리는 이유 |
